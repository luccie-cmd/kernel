#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/spinlock.h>
#include <cstdlib>
#include <cstring>
#include <kernel/hal/arch/x64/gdt/gdt.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <kernel/mmu/mmu.h>
#include <kernel/task/syscall.h>
#include <kernel/task/task.h>
#include <kernel/vfs/vfs.h>
#include <queue>

#define MODULE "Task manager"

uint64_t __attribute__((section(".trampoline.data"))) tempValue;
namespace task {
bool                 initialized   = false;
Process*             globalParent  = nullptr;
Process*             currentProc   = nullptr;
Thread*              currentThread = nullptr;
std::queue<Process*> zombieProcs;
pid_t                pids = 2;
std::Spinlock        currentProcLock;
std::Spinlock        currentThreadLock;
std::vector<GSbase*> cpuLocals;
void                 initialize() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    initialized = true;
    cpuLocals.resize(hal::arch::x64::irq::getMaxCPUs());
    for (size_t i = 0; i < hal::arch::x64::irq::getMaxCPUs(); ++i) {
        cpuLocals.at(i) = (GSbase*)(mmu::pmm::allocate() + mmu::vmm::getHHDM());
        std::memset(cpuLocals.at(i), 0, sizeof(cpuLocals.at(i)[0]));
        cpuLocals.at(i)->kernelCR3 = (uint64_t)mmu::vmm::getPML4(KERNEL_PID) - mmu::vmm::getHHDM();
    }
    dbg::popTrace();
}
bool isInitialized() {
    return initialized;
}
pid_t getNewPID() {
    if (!isInitialized()) {
        initialize();
    }
    return pids++;
}
static Process* findProcByPID(pid_t pid) {
    if (currentProc->pid == pid) {
        return currentProc;
    }
    Process* head = currentProc;
    while (head->pid != pid) {
        head = head->next;
        if (head == currentProc) {
            dbg::printm(MODULE, "Circular dependency exceeded, process %llu not found\n", pid);
            std::abort();
        }
    }
    return head;
}
static Thread* findThreadByTID(pid_t pid, pid_t tid) {
    Process* proc = findProcByPID(pid);
    Thread*  ret  = proc->threads;
    while (ret->tid != tid) {
        ret = ret->next;
        if (ret == proc->threads) {
            dbg::printm(MODULE,
                        "Circular dependency exceeded, thread %llu not found in process %llu\n",
                        tid, pid);
            std::abort();
        }
    }
    return ret;
}
static inline bool isInRange(uint64_t start, uint64_t end, uint64_t address) {
    return (address >= start && address < end);
}
void attachThread(pid_t pid, uint64_t entryPoint) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    Process*     proc      = findProcByPID(pid);
    const size_t stackSize = 4 * PAGE_SIZE;
    uint64_t     stackPhys = mmu::pmm::allocVirtual(stackSize);
    uint64_t     stackVirt = USER_STACK_TOP - stackSize;
    for (size_t i = 0; i < stackSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), stackPhys + i, stackVirt + i,
                          PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
    }
    Thread* thread = new Thread;
    thread->tid    = 0;
    std::memset(thread->fpuState, 0, sizeof(thread->fpuState));
    thread->registers =
        (io::Registers*)(mmu::pmm::allocVirtual(sizeof(io::Registers)) + mmu::vmm::getHHDM());
    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), (uint64_t)thread->registers - mmu::vmm::getHHDM(),
                      (uint64_t)thread->registers, PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
    std::memset(thread->registers, 0, sizeof(io::Registers));
    thread->registers->orig_rsp = stackVirt + stackSize - 16;
    thread->registers->rbp      = thread->registers->orig_rsp;
    thread->registers->rip      = entryPoint;
    thread->registers->rflags   = 0x202;
    thread->status              = ThreadStatus::Ready;
    thread->fsBase              = mmu::pmm::allocate() + mmu::vmm::getHHDM();
    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), (uint64_t)thread->fsBase, (uint64_t)thread->fsBase,
                      PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
    if (proc->threads == nullptr) {
        thread->next  = thread;
        proc->threads = thread;
    } else {
        Thread* current = proc->threads;
        while (current->next != proc->threads) {
            current = current->next;
        }
        thread->next  = proc->threads;
        current->next = thread;
    }
    dbg::popTrace();
}
extern "C" __attribute__((noreturn)) void switchProc(io::Registers* regs, mmu::vmm::PML4* pml4,
                                                     uint64_t fsBase);
extern "C" uint64_t                       __trampoline_text_start;
extern "C" uint64_t                       __trampoline_text_end;
extern "C" uint64_t                       __trampoline_data_start;
extern "C" uint64_t                       __trampoline_data_end;
extern "C" void                           printRegs(io::Registers* regs);
void                                      nextProc() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    currentProcLock.lock();
    currentThreadLock.lock();
    Process* beginProc = currentProc;
    bool     searching = true;
    do {
        if (currentProc->state != ProcessState::Blocked &&
            currentProc->state != ProcessState::Zombie) {
            Thread* beginThread = currentProc->threads;
            Thread* candidate   = beginThread;
            do {
                if (candidate->status == ThreadStatus::Ready) {
                    currentProc->threads = candidate;
                    currentProc->state = ProcessState::Running;
                    candidate->status  = ThreadStatus::Running;
                    searching          = false;
                    break;
                }
                candidate = candidate->next;
            } while (candidate != beginThread);
            if (!searching) {
                break;
            }
            dbg::printm(MODULE, "No runnable threads in PID %lu, skipping\n", currentProc->pid);
        }
        currentProc = currentProc->next;
        if (currentProc == beginProc) {
            dbg::printm(MODULE, "Ran out of processes to run\n");
            if (!zombieProcs.empty()) {
                dbg::printm(MODULE, "No processes left to cleanup zombie procs\n");
                std::abort();
            }
            currentProcLock.unlock();
            currentThreadLock.unlock();
            dbg::popTrace();
            return;
        }
    } while (searching);
    currentThread = currentProc->threads;
#ifdef DEBUG
    dbg::printm(MODULE, "%lu %lu\n", currentProc->state, currentThread->status);
#endif
    currentProc->state    = ProcessState::Running;
    currentThread->status = ThreadStatus::Running;
    if (!currentProc->hasStarted) {
        currentProc->hasStarted = true;
        uint64_t trampoline_va  = (uint64_t)&__trampoline_text_start;
        uint64_t trampoline_phys =
            mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        while (trampoline_va < (uint64_t)&__trampoline_text_end) {
            if (trampoline_phys == 0) {
                dbg::printm(MODULE, "Unable to find trampoline physical address!!!\n");
                std::abort();
            }
            mmu::vmm::mapPage(currentProc->pml4, trampoline_phys, trampoline_va, PROTECTION_RW,
                              MAP_PRESENT);
            trampoline_va += PAGE_SIZE;
            trampoline_phys =
                mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        }
        trampoline_va = (uint64_t)&__trampoline_data_start;
        trampoline_phys =
            mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        while (trampoline_va < (uint64_t)&__trampoline_data_end) {
            if (trampoline_phys == 0) {
                dbg::printm(MODULE, "Unable to find trampoline physical address!!!\n");
                std::abort();
            }
            mmu::vmm::mapPage(currentProc->pml4, trampoline_phys, trampoline_va, PROTECTION_RW,
                              MAP_PRESENT);
            trampoline_va += PAGE_SIZE;
            trampoline_phys =
                mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        }
    }
    GSbase* gsbase        = cpuLocals.at(hal::arch::x64::irq::getAPICID());
    gsbase->currentProc   = currentProc;
    gsbase->currentThread = currentThread;
    if (gsbase->stackTop) {
        mmu::pmm::free(gsbase->stackTop - mmu::vmm::getHHDM(), PAGE_SIZE);
    }
    gsbase->stackTop = mmu::pmm::allocate() + mmu::vmm::getHHDM();
    if (mmu::vmm::getPhysicalAddr(currentProc->pml4, (uint64_t)gsbase, false, false) == 0) {
        mmu::vmm::mapPage(currentProc->pml4, (uint64_t)gsbase - mmu::vmm::getHHDM(),
                          (uint64_t)gsbase, PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW,
                          MAP_PRESENT);
    }
    io::wrmsr(0xC0000102, (uint64_t)gsbase);
    hal::arch::x64::gdt::mapStacksToProc(currentProc->pid, currentProc->pml4);
    if ((mmu::vmm::getPhysicalAddr(currentProc->pml4, (uint64_t)switchProc & (~0xFFF), true)) ==
        0) {
        dbg::printm(MODULE, "switchProc(io::Registers*) became unmapped!!!\n");
        std::abort();
    }
    // dbg::printm(MODULE, "Registers for PID %lu TID %lu\n", currentProc->pid, currentThread->tid);
    // printRegs(currentThread->registers);
    // dbg::printf("\n");
    io::Registers*  regs = currentThread->registers;
    mmu::vmm::PML4* pml4 = reinterpret_cast<mmu::vmm::PML4*>(
        reinterpret_cast<uint64_t>(currentProc->pml4) - mmu::vmm::getHHDM());
    uint64_t fsBase = currentThread->fsBase;
    dbg::popTrace();
    currentProcLock.unlock();
    currentThreadLock.unlock();
    switchProc(regs, pml4, fsBase);
}
void printRfl(uint64_t rflags) {
    if (rflags & 0x00000001) dbg::print("CF ");
    if (rflags & 0x00000004) dbg::print("PF ");
    if (rflags & 0x00000010) dbg::print("AF ");
    if (rflags & 0x00000040) dbg::print("ZF ");
    if (rflags & 0x00000080) dbg::print("SF ");
    if (rflags & 0x00000100) dbg::print("TF ");
    if (rflags & 0x00000200) dbg::print("IF ");
    if (rflags & 0x00000400) dbg::print("DF ");
    if (rflags & 0x00000800) dbg::print("OF ");
    if (rflags & 0x00010000) dbg::print("RF ");
    if (rflags & 0x00020000) dbg::print("VM ");
    if (rflags & 0x00040000) dbg::print("AC ");
    if (rflags & 0x00080000) dbg::print("VIF ");
    if (rflags & 0x00100000) dbg::print("VIP ");
    if (rflags & 0x00200000) dbg::print("ID ");
    if (rflags & 0x80000000) dbg::print("AI ");
    dbg::print("\n");
}
std::Spinlock printLock;
void          printRegs(syscall::SyscallRegs* regs) {
    printLock.lock();
    dbg::printf("RBX=0x%016.16llx RDX=0x%016.16llx\n", regs->rbx, regs->rdx);
    dbg::printf("RSI=0x%016.16llx RDI=0x%016.16llx RBP=0x%016.16llx RSP=0x%016.16llx\n", regs->rsi,
                         regs->rdi, regs->rbp, tempValue);
    dbg::printf("R8 =0x%016.16llx R9 =0x%016.16llx RAX=0x%016.16llx\n", regs->r8, regs->r9,
                         regs->rax);
    dbg::printf("R12=0x%016.16llx R13=0x%016.16llx R14=0x%016.16llx R15=0x%016.16llx\n", regs->r12,
                         regs->r13, regs->r14, regs->r15);
    dbg::printf("RIP=0x%016.16llx RFL=", regs->rip);
    printRfl(regs->rflags);
    dbg::printf("CR2=0x%016.16llx\n", io::rcr2());
    printLock.unlock();
}
void unblockProcess(Process* proc) {
    dbg::printm(MODULE, "TODO: Unblock process %lu\n", proc->pid);
    std::abort();
}
void blockProcess(Process* proc) {
    dbg::printm(MODULE, "TODO: Block process %lu\n", proc->pid);
    std::abort();
}
void sendSignal(Process* proc, size_t signal) {
    if (proc->state != ProcessState::Ready && proc->state != ProcessState::Running) {
        dbg::printm(MODULE, "Attempted to send signal to blocked or zombie process\n");
        std::abort();
    }
    if (proc->signals.find(signal) == proc->signals.end()) {
        dbg::printm(MODULE, "Attempted to send invalid signal %llu\n", signal);
        std::abort();
    }
    proc->signals.at(signal)(signal);
}
void cleanProc(pid_t pid, uint8_t exitCode) {
    Process* exitProc = findProcByPID(pid);
    dbg::printf("Process %u has exited (code %u)\n", exitProc->pid, exitCode);
    if (pid == INIT_PID) {
        dbg::printm(MODULE, "Init tried exiting\n");
        std::abort();
    } else {
        if (exitProc->parent) {
            if (exitProc->parent->waitingFor == exitProc->pid) {
                exitProc->parent->waitingFor = 0;
                exitProc->parent->waitStatus = exitCode;
                unblockProcess(exitProc->parent);
            }
            exitProc->state    = ProcessState::Zombie;
            exitProc->exitCode = exitCode;
            sendSignal(exitProc->parent, SIGCHILD);
            zombieProcs.push(exitProc);
        } else {
            exitProc->next->prev = exitProc->prev;
            exitProc->prev->next = exitProc->next;
            for (Process* proc : exitProc->children) {
                proc->parent = globalParent;
                globalParent->children.push_back(proc);
                if (proc->state == ProcessState::Zombie) {
                    cleanProc(proc->pid, proc->exitCode);
                }
            }
            dbg::printf("TODO: Free process resources\n");
        }
    }
}
void cleanThread(pid_t pid, pid_t tid, uint8_t exitCode) {
    dbg::printf("TODO: Free thread resources\n");
    Thread* exitThread   = findThreadByTID(pid, tid);
    exitThread->status   = ThreadStatus::Zombie;
    exitThread->exitCode = exitCode;
}
size_t sysWriteScreen(syscall::SyscallRegs* regs) {
    syscall::SyscallRegs* tempRegs = new syscall::SyscallRegs;
    tempRegs->rdi                  = 1;
    tempRegs->rsi                  = regs->rdi;
    tempRegs->rdx                  = regs->rsi;
    size_t result                  = syscall::sysWrite(tempRegs);
    delete tempRegs;
    return result;
}
using sysFunc                                    = std::function<size_t(syscall::SyscallRegs*)>;
std::unordered_map<size_t, sysFunc> sysFunctions = {{SYSCALL_EXIT, syscall::sysExit},
                                                    {SYSCALL_WRITE, syscall::sysWrite},
                                                    {SYSCALL_WRITESCREEN, sysWriteScreen}};
extern "C" void                     syscallHandler(syscall::SyscallRegs* regs) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    currentThread->status              = ThreadStatus::Blocked;
    currentThread->registers->rax      = regs->rax;
    currentThread->registers->rbx      = regs->rbx;
    currentThread->registers->rip      = regs->rip;
    currentThread->registers->rdx      = regs->rdx;
    currentThread->registers->rsi      = regs->rsi;
    currentThread->registers->rdi      = regs->rdi;
    currentThread->registers->rbp      = regs->rbp;
    currentThread->registers->orig_rsp = tempValue;
    currentThread->registers->r8       = regs->r8;
    currentThread->registers->r9       = regs->r9;
    currentThread->registers->r12      = regs->r12;
    currentThread->registers->r13      = regs->r13;
    currentThread->registers->r14      = regs->r14;
    currentThread->registers->r15      = regs->r15;
    currentThread->registers->rflags   = regs->rflags;
    __asm__("mfence" : : : "memory");
    // dbg::printf("\nRegs:\n");
    // printRegs(regs);
    // dbg::printf("\n");
    if (sysFunctions.find(regs->rax) == sysFunctions.end()) {
        printRegs(regs);
        dbg::printm(MODULE,
                                        "TODO: Process used invalid (Or unimplemented) syscall, sending SIGKILL\n");
        sendSignal(currentProc, SIGKILL);
        __builtin_unreachable();
    }
    sysFunc func                  = sysFunctions.at(regs->rax);
    currentThread->registers->rax = func(regs);
    dbg::popTrace();
    nextProc();
}
Process* getCurrentProc() {
    GSbase* base = cpuLocals.at(hal::arch::x64::irq::getAPICID());
    return base->currentProc;
}
Thread* getCurrentThread() {
    GSbase* base = cpuLocals.at(hal::arch::x64::irq::getAPICID());
    return base->currentThread;
}
}; // namespace task