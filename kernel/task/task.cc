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
pid_t                pids = 1;
std::Spinlock        currentProcLock;
std::Spinlock        currentThreadLock;
struct GSbase {
    Thread*  currentThread; // 0x00
    Process* currentProc;   // 0x08
    uint64_t kernelCR3;     // 0x10
    uint64_t stackTop;      // 0x18
} __attribute__((packed));
std::vector<GSbase*> cpuLocals;
void                 initialize() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    initialized = true;
    cpuLocals.resize(hal::arch::x64::irq::getMaxCPUs());
    for (size_t i = 0; i < hal::arch::x64::irq::getMaxCPUs(); ++i) {
        cpuLocals.at(i) = (GSbase*)(mmu::pmm::allocate() + mmu::vmm::getHHDM());
        cpuLocals.at(i)->kernelCR3 = (uint64_t)mmu::vmm::getPML4(KERNEL_PID) - mmu::vmm::getHHDM();
    }
    pids = 10;
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
static Process* findProcByPml4(mmu::vmm::PML4* pml4) {
    currentProcLock.lock();
    if (currentProc->pml4 == pml4) {
        currentProcLock.unlock();
        return currentProc;
    }
    Process* head = currentProc;
    while (head->pml4 != pml4) {
        head = head->next;
        if (head == currentProc) {
            dbg::printm(MODULE, "Circular dependency exceeded, no process found for PML4 0x%llx\n",
                        pml4);
            std::abort();
        }
    }
    currentProcLock.unlock();
    return head;
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
static ProcessMemoryMapping* findMappingInProcess(Process* proc, uint64_t virtualAddress) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    ProcessMemoryMapping* head = proc->memoryMapping;
    while (head) {
        if (isInRange(head->virtualStart, head->virtualStart + head->memLength, virtualAddress)) {
            dbg::popTrace();
            return head;
        }
        head = head->next;
    }
    dbg::popTrace();
    return nullptr;
}
static std::vector<Elf64_Rela*> findRelasInPage(Process* proc, uint64_t virtualAddr) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    std::vector<Elf64_Rela*> newRelas;
    for (Elf64_Rela* rela : proc->relas) {
        if (isInRange(virtualAddr, virtualAddr + PAGE_SIZE, rela->r_offset)) {
            newRelas.push_back(rela);
        }
    }
    dbg::popTrace();
    return newRelas;
}
std::Spinlock mapProcLock;
void          mapProcess(mmu::vmm::PML4* pml4, uint64_t virtualAddress) {
    mapProcLock.lock();
    dbg::addTrace(__PRETTY_FUNCTION__);
    Process* proc = findProcByPml4(pml4);
    proc->state   = ProcessState::Blocked;
    cpuLocals.at(hal::arch::x64::irq::getAPICID())->currentThread->status = ThreadStatus::Blocked;
    ProcessMemoryMapping* mapping = findMappingInProcess(proc, virtualAddress);
    if (mapping == nullptr) {
        std::abort();
    }
    if (mapping->fileIdx == static_cast<size_t>(-1)) {
        dbg::printm(MODULE, "TODO: Map 0x%llx as data\n", virtualAddress);
        std::abort();
    } else {
        uint8_t* bufferAddr = (uint8_t*)mmu::pmm::allocate();
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)bufferAddr, (uint64_t)bufferAddr,
                                   PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
        size_t    remaining   = PAGE_SIZE;
        uintptr_t base        = ALIGNDOWN(virtualAddress, PAGE_SIZE);
        size_t    i           = 0;
        int       permissions = 0;
        while (remaining > 0) {
            mapping = findMappingInProcess(proc, base + i);
            if (mapping == nullptr) {
                bufferAddr[i++] = 0;
                remaining--;
                continue;
            }
            if (mapping->fileIdx == static_cast<size_t>(-1)) {
                dbg::printm(MODULE, "TODO: Map 0x%llx as data\n", base + i);
                std::abort();
            }
            uint64_t savedOffset = vfs::getOffset(mapping->fileIdx);
            size_t   readSize    = std::min(mapping->fileLength, remaining);
            vfs::seek(mapping->fileIdx, mapping->fileOffset + (base + i - mapping->virtualStart));
            vfs::readFile(mapping->fileIdx, readSize, (void*)(bufferAddr + i));
            vfs::seek(mapping->fileIdx, savedOffset);
            permissions |= mapping->permissions;
            remaining -= readSize;
            i += readSize;
        }
        if ((permissions & PROTECTION_RW) == 0) {
            dbg::printm(MODULE,
                                 "ERROR: No read or write permissions set for virtual address 0x%llu in PID "
                                          "%llu. TOOD: sysExit\n",
                                 virtualAddress, proc->pid);
            std::abort();
        }
        std::vector<Elf64_Rela*> toHandleRelas = findRelasInPage(proc, base);
        for (Elf64_Rela* rela : toHandleRelas) {
            switch (ELF64_R_TYPE(rela->r_info)) {
            case R_X86_64_RELATIVE: {
                *(uint64_t*)(bufferAddr + (rela->r_offset & 0xFFF)) =
                    proc->baseAddr + rela->r_addend;
            } break;
            default: {
                dbg::printm(MODULE,
                                     "TODO: Handle rela 0x%llx type: %llu sym: %llu addend: 0x%llx\n",
                                     rela->r_offset, ELF64_R_TYPE(rela->r_info), ELF64_R_SYM(rela->r_info),
                                     rela->r_addend);
                std::abort();
            } break;
            }
        }
        uint64_t phys = mmu::pmm::allocate();
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), phys, base,
                                   PROTECTION_KERNEL | permissions, MAP_PRESENT);
        mmu::vmm::mapPage(proc->pml4, phys, base, permissions, MAP_PRESENT);
        std::memcpy((void*)base, bufferAddr, PAGE_SIZE);
        mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), base);
        mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)bufferAddr);
        mmu::pmm::free((uint64_t)bufferAddr, PAGE_SIZE);
    }
    cpuLocals.at(hal::arch::x64::irq::getAPICID())->currentThread->status = ThreadStatus::Ready;
    proc->state = ProcessState::Ready;
    mapProcLock.unlock();
    dbg::popTrace();
}
void attachThread(pid_t pid, uint64_t entryPoint) {
    Process*     proc      = findProcByPID(pid);
    const size_t stackSize = 4 * PAGE_SIZE;
    uint64_t     stackPhys = mmu::pmm::allocVirtual(stackSize);
    uint64_t     stackVirt = stackPhys;
    for (size_t i = 0; i < stackSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), stackPhys + i, stackVirt + i,
                          PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
    }
    Thread* thread = new Thread;
    thread->tid    = 0;
    std::memset(thread->fpuState, 0, sizeof(thread->fpuState));
    thread->registers = (io::Registers*)mmu::pmm::allocVirtual(sizeof(io::Registers));
    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), (uint64_t)thread->registers,
                      (uint64_t)thread->registers, PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
    mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)thread->registers,
                      (uint64_t)thread->registers,
                      PROTECTION_NOEXEC | PROTECTION_RW | PROTECTION_KERNEL, MAP_PRESENT);
    std::memset(thread->registers, 0, sizeof(io::Registers));
    thread->registers->orig_rsp = stackVirt + stackSize - 16;
    thread->registers->rbp      = thread->registers->orig_rsp;
    thread->registers->rip      = entryPoint;
    thread->registers->rflags   = 0x202;
    thread->status              = ThreadStatus::Ready;
    thread->fsBase              = mmu::pmm::allocate();
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
}
void __attribute__((section(".trampoline.text"))) defaultSignalHandler(size_t signal) {
    switch (signal) {
    case SIGABORT: {
        cleanProc(getCurrentProc()->pid, 1);
        nextProc();
        __builtin_unreachable();
    } break;
    default: {
        dbg::printm(MODULE, "TODO: Handle signal %lu\n", signal);
        std::abort();
    }
    }
}
void makeNewProcess(pid_t pid, uint64_t entryPoint, size_t fileIdx, Elf64_Addr baseAddr,
                    std::vector<Mapping*> mappings, Elf64_Addr relaVirtual, Elf64_Xword relaSize) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    Process* proc = new Process;
    proc->pid     = pid;
    proc->state   = ProcessState::Ready;
    proc->pml4    = mmu::vmm::getPML4(pid);
    proc->signals.insert({SIGABORT, (signalHandler)defaultSignalHandler});
    if (!relaVirtual) {
        dbg::printm(MODULE, "WARNING: No RELA address was passed in\n");
    }
    proc->baseAddr = baseAddr;
    proc->threads  = nullptr;
    for (Mapping* mapping : mappings) {
        ProcessMemoryMapping* memMapping = new ProcessMemoryMapping;
        memMapping->fileIdx              = fileIdx;
        memMapping->fileOffset           = mapping->fileOffset;
        memMapping->memLength            = mapping->memLength;
        memMapping->fileLength           = mapping->fileLength;
        memMapping->permissions          = mapping->permissions;
        memMapping->virtualStart         = mapping->virtualStart;
        memMapping->alignment            = mapping->alignment;
        size_t    offset                 = memMapping->virtualStart % PAGE_SIZE;
        size_t    totalLen               = ALIGNUP(memMapping->memLength + offset, PAGE_SIZE);
        uintptr_t mapBase                = ALIGNDOWN(memMapping->virtualStart, PAGE_SIZE);
        if (memMapping->virtualStart % PAGE_SIZE == 0) {
            for (size_t i = 0; i < totalLen; i += PAGE_SIZE) {
                mmu::vmm::mapPage(mmu::vmm::getPML4(pid), ONDEMAND_MAP_ADDRESS, mapBase + i,
                                  memMapping->permissions, 0);
            }
        } else {
            ProcessMemoryMapping* procMapping =
                findMappingInProcess(proc, ALIGNDOWN(mapping->virtualStart, mapping->alignment));
            if (procMapping == nullptr) {
                for (size_t i = 0; i < totalLen; i += PAGE_SIZE) {
                    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), ONDEMAND_MAP_ADDRESS, mapBase + i,
                                      memMapping->permissions, 0);
                }
            } else {
                dbg::printm(MODULE, "TODO: Further memory permisssion sharing\n");
                std::abort();
            }
        }
        dbg::printm(MODULE,
                    "Added new mapping to process %llu (VADDR = 0x%llx Mapped Length = 0x%llx)\n",
                    proc->pid, memMapping->virtualStart, memMapping->memLength);
        memMapping->next    = proc->memoryMapping;
        proc->memoryMapping = memMapping;
    }
    if (relaVirtual) {
        ProcessMemoryMapping* mapping = findMappingInProcess(proc, relaVirtual);
        for (size_t i = 0; i < relaSize; i += sizeof(Elf64_Rela)) {
            Elf64_Rela* rela = new Elf64_Rela;
            vfs::seek(mapping->fileIdx, (relaVirtual - proc->baseAddr) + mapping->fileOffset +
                                            i * sizeof(Elf64_Rela));
            vfs::readFile(mapping->fileIdx, sizeof(Elf64_Rela), rela);
            rela->r_offset += proc->baseAddr;
            proc->relas.push_back(rela);
        }
        dbg::printm(MODULE, "Added %llu rela entries\n", proc->relas.size());
    }
    if (globalParent == nullptr) {
        globalParent = proc;
    }
    proc->parent = currentProc;
    if (currentProc == nullptr) {
        currentProc = proc;
        proc->next  = proc;
        proc->prev  = proc;
    } else {
        currentProc->children.push_back(proc);
        proc->prev              = currentProc->prev;
        proc->next              = currentProc;
        currentProc->prev->next = proc;
        currentProc->prev       = proc;
    }
    attachThread(pid, entryPoint);
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
    dbg::printm(MODULE, "%lu %lu\n", currentProc->state, currentThread->status);
    currentProc->state    = ProcessState::Running;
    currentThread->status = ThreadStatus::Running;
    if (!currentProc->hasStarted) {
        currentProc->hasStarted = true;
        uint64_t trampoline_va = (uint64_t)&__trampoline_text_start;
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
    GSbase* gsbase = cpuLocals.at(hal::arch::x64::irq::getAPICID());
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
    dbg::printm(MODULE, "%lu: Choose proc %lu and tid %lu\n", hal::arch::x64::irq::getAPICID(),
                                                     currentProc->pid, currentThread->tid);
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
}
void blockProcess(Process* proc) {
    dbg::printm(MODULE, "TODO: Block process %lu\n", proc->pid);
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
    dbg::printf("TODO: Free process resources\n");
    Process* exitProc = findProcByPID(pid);
    dbg::printf("Process %u has exited (code %u)\n", exitProc->pid, exitCode);
    if (exitProc->parent) {
        if (exitProc->parent->waitingFor == exitProc->pid) {
            exitProc->parent->waitingFor = 0;
            exitProc->parent->waitStatus = exitCode;
            unblockProcess(exitProc->parent);
        }
        sendSignal(exitProc->parent, SIGCHILD);
    }
    exitProc->state    = ProcessState::Zombie;
    exitProc->exitCode = exitCode;
    zombieProcs.push(exitProc);
}
void cleanThread(pid_t pid, pid_t tid, uint8_t exitCode) {
    dbg::printf("TODO: Free thread resources\n");
    Thread* exitThread   = findThreadByTID(pid, tid);
    exitThread->status   = ThreadStatus::Dead;
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
                                        "TODO: Process used invalid (Or unimplemented) syscall, sending SIGABORT\n");
        sendSignal(currentProc, SIGABORT);
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