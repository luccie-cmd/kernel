#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstring>
#include <kernel/hal/arch/x64/gdt/gdt.h>
#include <kernel/mmu/mmu.h>
#include <kernel/task/task.h>
#include <kernel/vfs/vfs.h>
#define MODULE "Task manager"

uint64_t __attribute__((section(".trampoline.data")))  tempValue;
uint64_t* __attribute__((section(".trampoline.data"))) tempStack;
namespace task {
bool                  initialized   = false;
Process*              globalParent  = nullptr;
Process*              currentProc   = nullptr;
Thread*               currentThread = nullptr;
std::vector<Process*> zombieProcs;
pid_t                 pids = 1;
extern "C" void       syscallEntry();
void                  initialize() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    initialized = true;
    pids        = 10;
    io::wrmsr(0xC0000082, (uint64_t)syscallEntry);
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
    if (currentProc->pml4 == pml4) {
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
static inline bool isInRange(uint64_t start, uint64_t end, uint64_t address) {
    return (address >= start && address <= end);
}
static ProcessMemoryMapping* findMappingInProcess(Process* proc, uint64_t virtualAddress) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    ProcessMemoryMapping* head = proc->memoryMapping;
    while (head) {
        if (isInRange(head->virtualStart, head->virtualStart + head->length, virtualAddress)) {
            dbg::popTrace();
            return head;
        }
        head = head->next;
    }
    dbg::printm(MODULE, "Couldn't find memory mapping for address 0x%llx\n", virtualAddress);
    std::abort();
}
void mapProcess(mmu::vmm::PML4* pml4, uint64_t virtualAddress) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    virtualAddress &= (virtualAddress + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    Process*              proc    = findProcByPml4(pml4);
    ProcessMemoryMapping* mapping = findMappingInProcess(proc, virtualAddress);
    if (mapping->fileIdx == static_cast<size_t>(-1)) {
        dbg::printm(MODULE, "TODO: Map 0x%llx as data\n", virtualAddress);
        std::abort();
    } else {
        uint64_t bufferAddr = mmu::pmm::allocate();
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), bufferAddr, bufferAddr,
                          PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
        uint64_t offset = vfs::getOffset(mapping->fileIdx);
        vfs::seek(mapping->fileIdx, (virtualAddress - mapping->virtualStart) + mapping->fileOffset);
        vfs::readFile(mapping->fileIdx,
                      std::min((uint64_t)PAGE_SIZE, (mapping->fileOffset + mapping->length) -
                                                        vfs::getOffset(mapping->fileIdx)),
                      (void*)bufferAddr);
        vfs::seek(mapping->fileIdx, offset);
        uint64_t phys = mmu::pmm::allocate();
        mmu::vmm::mapPage(proc->pml4, phys, virtualAddress, mapping->permissions, MAP_PRESENT);
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), phys, virtualAddress,
                          PROTECTION_KERNEL | mapping->permissions, MAP_PRESENT);
        std::memcpy((void*)virtualAddress, (void*)bufferAddr, PAGE_SIZE);
        mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)virtualAddress);
        mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), bufferAddr);
        mmu::pmm::free(bufferAddr);
        dbg::printm(MODULE,
                    "On demand map for process %llu 0x%llx to 0x%llx physical (using 0x%llx as "
                    "temporary)\n",
                    proc->pid, virtualAddress, phys, bufferAddr);
    }
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
void makeNewProcess(pid_t pid, uint64_t entryPoint, size_t fileIdx,
                    std::vector<Mapping*> mappings) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    Process* proc = new Process;
    proc->pid     = pid;
    proc->state   = ProcessState::Ready;
    proc->pml4    = mmu::vmm::getPML4(pid);
    proc->threads = nullptr;
    for (Mapping* mapping : mappings) {
        ProcessMemoryMapping* memMapping = new ProcessMemoryMapping;
        memMapping->fileIdx              = fileIdx;
        memMapping->fileOffset           = mapping->fileOffset;
        memMapping->length               = mapping->length;
        memMapping->permissions          = mapping->permissions;
        memMapping->virtualStart         = mapping->virtualStart;
        if (memMapping->virtualStart % PAGE_SIZE == 0) {
            for (size_t i = 0; i < ALIGNUP(memMapping->length, PAGE_SIZE); i += PAGE_SIZE) {
                mmu::vmm::mapPage(mmu::vmm::getPML4(pid), ONDEMAND_MAP_ADDRESS,
                                  memMapping->virtualStart + i, memMapping->permissions, 0);
            }
        } else {
            dbg::printm(MODULE, "TODO: Memory permisssion sharing\n");
            std::abort();
        }
        memMapping->next    = proc->memoryMapping;
        proc->memoryMapping = memMapping;
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
extern "C" void     switchProc(io::Registers* regs, mmu::vmm::PML4* pml4);
extern "C" uint64_t __trampoline_text_start;
extern "C" uint64_t __trampoline_text_end;
extern "C" uint64_t __trampoline_data_start;
extern "C" uint64_t __trampoline_data_end;
extern "C" void     printRegs(io::Registers* regs);
void                nextProc() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    Process* beginProc = currentProc;
    while (currentProc->state == ProcessState::Blocked ||
           currentProc->state == ProcessState::Zombie) {
        currentProc = currentProc->next;
        if (currentProc == beginProc) {
            dbg::printm(MODULE, "Ran out of processes to run\n");
            std::abort();
        }
        Thread* beginThread = currentProc->threads;
        while (currentProc->threads->status == ThreadStatus::Blocked) {
            currentProc->threads = currentProc->threads->next;
            if (currentProc->threads == beginThread) {
                dbg::printm(MODULE, "Ran out of threads to run, switching to next process\n");
                std::abort();
            }
        }
    }
    currentThread = currentProc->threads;
    if (!currentProc->hasStarted) {
        currentProc->hasStarted = true;
        uint64_t trampoline_va  = (uint64_t)&__trampoline_text_start;
        uint64_t trampoline_phys =
            mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        while (trampoline_va < (uint64_t)&__trampoline_text_end) {
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
            mmu::vmm::mapPage(currentProc->pml4, trampoline_phys, trampoline_va, PROTECTION_RW,
                                             MAP_PRESENT);
            trampoline_va += PAGE_SIZE;
            trampoline_phys =
                mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        }
        hal::arch::x64::gdt::setRSP0(currentProc->pid);
    }
    if ((mmu::vmm::getPhysicalAddr(currentProc->pml4, (uint64_t)switchProc & (~0xFFF), true)) ==
        0) {
        dbg::printm(MODULE, "switchProc(io::Registers*) became unmapped!!!\n");
        std::abort();
    }
    dbg::printm(MODULE, "Registers for PID %lu TID %lu\n", currentProc->pid, currentThread->tid);
    printRegs(currentThread->registers);
    dbg::printf("\n");
    dbg::popTrace();
    switchProc(currentThread->registers,
                              reinterpret_cast<mmu::vmm::PML4*>(reinterpret_cast<uint64_t>(currentProc->pml4) -
                                                                mmu::vmm::getHHDM()));
}
struct SyscallRegs {
    uint64_t rbx, rip, rdx, rbp, rsi, rdi, r8, r9, r10, rflags, r12, r13, r14, r15, cr3, rax;
} __attribute__((packed));
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
void printRegs(SyscallRegs* regs) {
    dbg::printf("RAX=0x%016.16llx RBX=0x%016.16llx RDX=0x%016.16llx\n", regs->rax, regs->rbx,
                regs->rdx);
    dbg::printf("RSI=0x%016.16llx RDI=0x%016.16llx RBP=0x%016.16llx RSP=0x%016.16llx\n", regs->rsi,
                regs->rdi, regs->rbp, tempValue);
    dbg::printf("R8 =0x%016.16llx R9 =0x%016.16llx R10=0x%016.16llx\n", regs->r8, regs->r9,
                regs->r10);
    dbg::printf("R12=0x%016.16llx R13=0x%016.16llx R14=0x%016.16llx R15=0x%016.16llx\n", regs->r12,
                regs->r13, regs->r14, regs->r15);
    dbg::printf("RIP=0x%016.16llx RFL=", regs->rip);
    printRfl(regs->rflags);
    dbg::printf("CR2=0x%016.16llx CR3=0x%016.16llx\n", io::rcr2(), regs->cr3);
}
static void unblockProcess(Process* proc) {
    dbg::printm(MODULE, "TODO: Unblock process %lu\n", proc->pid);
}
static void blockProcess(Process* proc) {
    dbg::printm(MODULE, "TODO: Block process %lu\n", proc->pid);
}
extern "C" void syscallHandler(SyscallRegs* regs) {
    dbg::addTrace(__PRETTY_FUNCTION__);
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
    currentThread->registers->r10      = regs->r10;
    currentThread->registers->r12      = regs->r12;
    currentThread->registers->r13      = regs->r13;
    currentThread->registers->r14      = regs->r14;
    currentThread->registers->r15      = regs->r15;
    currentThread->registers->rflags   = regs->rflags;
    currentThread->registers->cr3      = regs->cr3;
    currentThread->status              = ThreadStatus::Blocked;
    __asm__("mfence" : : : "memory");
    printRegs(regs);
    switch (regs->rax) {
    // SYS_exit
    case 0x0: {
        uint8_t exitCode = regs->rdi;
        // Last thread
        if (currentThread->next == currentThread && currentProc->threads == currentThread) {
            dbg::printm(MODULE, "TODO: Free process resources\n");
            if (currentProc->parent && currentProc->parent->waitingFor == currentProc->pid) {
                currentProc->parent->waitingFor = 0;
                currentProc->parent->waitStatus = exitCode;
                unblockProcess(currentProc->parent);
            }
            currentProc->state    = ProcessState::Zombie;
            currentProc->exitCode = exitCode;
            zombieProcs.push_back(currentProc);
        } else {
            dbg::printm(MODULE, "TODO: Support multiple thread exit\n");
            std::abort();
        }
        dbg::printm(MODULE, "TODO: Free thread resources\n");
    } break;
    // SYS_write
    case 0x1: {
        if (regs->rsi == 0) {
            break;
        }
        uint64_t tempRdi   = regs->rdi;
        uint64_t printSize = regs->rsi;
        char*    buffer    = new char[regs->rsi + 1];
        for (size_t i = 0; i < ALIGNUP(regs->rsi, PAGE_SIZE);
             i += PAGE_SIZE, tempRdi += PAGE_SIZE) {
            uint64_t physicalAddr =
                mmu::vmm::getPhysicalAddr(currentProc->pml4, tempRdi, true, true);
            if (physicalAddr == 0) {
                dbg::printm(MODULE, "Segfault, physical address was 0 and not mapped\n");
                std::abort();
            }
            if (physicalAddr == ONDEMAND_MAP_ADDRESS) {
                mapProcess(currentProc->pml4, tempRdi);
                physicalAddr = mmu::vmm::getPhysicalAddr(currentProc->pml4, tempRdi, false, false);
            }
            mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), physicalAddr, tempRdi,
                              PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
            std::memcpy(buffer, (void*)tempRdi, (std::min(regs->rsi + 1, (uint64_t)PAGE_SIZE)));
            if ((int64_t)regs->rsi - PAGE_SIZE < 0) {
                regs->rsi = 0;
            } else {
                regs->rsi -= PAGE_SIZE;
            }
            mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), tempRdi);
        }
        dbg::printf("%.*s\n", printSize, buffer);
        delete[] buffer;
        currentThread->status = ThreadStatus::Ready;
    } break;
    // SYS_waitpid
    default: {
        // printRegs(regs);
        printRegs(currentThread->registers);
        dbg::printm(MODULE, "TODO: Handle syscall %lu\n", regs->rax);
        std::abort();
    } break;
    }
    dbg::popTrace();
    nextProc();
}
}; // namespace task