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
bool            initialized   = false;
Process*        currentProc   = nullptr;
Thread*         currentThread = nullptr;
pid_t           pids          = 1;
extern "C" void syscallEntry();
void            initialize() {
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
    virtualAddress &= ~(PAGE_SIZE - 1);
    Process*              proc    = findProcByPml4(pml4);
    ProcessMemoryMapping* mapping = findMappingInProcess(proc, virtualAddress);
    if (mapping->fileIdx == static_cast<size_t>(-1)) {
        dbg::printm(MODULE, "TODO: Map 0x%llx as data\n", virtualAddress);
        std::abort();
    } else {
        uint8_t* buffer = new uint8_t[PAGE_SIZE];
        uint64_t offset = vfs::getOffset(mapping->fileIdx);
        vfs::seek(mapping->fileIdx, (virtualAddress - mapping->virtualStart) + mapping->fileOffset);
        vfs::readFile(mapping->fileIdx, PAGE_SIZE, buffer);
        vfs::seek(mapping->fileIdx, offset);
        uint64_t phys = mmu::pmm::allocate();
        mmu::vmm::mapPage(proc->pml4, phys, virtualAddress, mapping->permissions, MAP_PRESENT);
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), phys, virtualAddress,
                          PROTECTION_KERNEL | mapping->permissions, MAP_PRESENT);
        std::memcpy((void*)virtualAddress, buffer, PAGE_SIZE);
        mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)phys);
        dbg::printm(MODULE, "On demand map for process %llu 0x%llx to 0x%llx physical\n", proc->pid,
                    virtualAddress, phys);
        delete[] buffer;
    }
    dbg::popTrace();
}
void makeNewProcess(pid_t pid, uint64_t codeSize, uint64_t entryPoint, size_t fileIdx,
                    std::vector<std::pair<std::pair<size_t, size_t>, size_t>> mappings) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    uint64_t     alignedCodeSize = (codeSize + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    const size_t stackSize       = 4 * PAGE_SIZE;
    uint64_t     stackPhys       = mmu::pmm::allocVirtual(stackSize);
    uint64_t     stackVirt       = USER_STACK_TOP - stackSize;
    for (size_t i = 0; i < stackSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), stackPhys + i, stackVirt + i, PROTECTION_RW,
                          MAP_PRESENT);
    }
    uint64_t codeVirt = entryPoint;
    for (size_t i = 0; i < alignedCodeSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), ONDEMAND_MAP_ADDRESS, codeVirt + i, PROTECTION_RW,
                          0);
    }
    Thread* thread = new Thread;
    thread->tid    = 0;
    std::memset(thread->fpuState, 0, sizeof(thread->fpuState));
    thread->registers = (io::Registers*)mmu::pmm::allocVirtual(sizeof(io::Registers));
    std::memset(thread->registers, 0, sizeof(io::Registers));
    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), (uint64_t)thread->registers,
                      (uint64_t)thread->registers, PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
    thread->registers->orig_rsp = stackVirt + stackSize - 16;
    thread->registers->rbp      = thread->registers->orig_rsp;
    thread->registers->rip      = entryPoint;
    thread->registers->rflags   = 0x202;
    thread->isRunning           = false;
    thread->next                = thread;
    Process* proc               = new Process;
    proc->pid                   = pid;
    proc->pml4                  = mmu::vmm::getPML4(pid);
    proc->threads               = thread;
    for (std::pair<std::pair<size_t, size_t>, size_t> mapping : mappings) {
        ProcessMemoryMapping* memMapping = new ProcessMemoryMapping;
        memMapping->fileIdx              = fileIdx;
        memMapping->fileOffset           = mapping.first.second;
        memMapping->length               = mapping.second;
        memMapping->permissions          = PROTECTION_RW;
        memMapping->virtualStart         = mapping.first.first;
        memMapping->next                 = proc->memoryMapping;
        proc->memoryMapping              = memMapping;
    }
    if (currentProc == nullptr) {
        currentProc = proc;
        proc->next  = proc;
        proc->prev  = proc;
    } else {
        proc->prev              = currentProc->prev;
        proc->next              = currentProc;
        currentProc->prev->next = proc;
        currentProc->prev       = proc;
    }
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
    currentProc          = currentProc->next;
    currentProc->threads = currentProc->threads->next;
    currentThread        = currentProc->threads;
    if (currentProc->hasStarted == false) {
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
    dbg::popTrace();
    switchProc(currentThread->registers,
                              reinterpret_cast<mmu::vmm::PML4*>(reinterpret_cast<uint64_t>(currentProc->pml4) -
                                                                mmu::vmm::getHHDM()));
}
struct __attribute__((packed)) SyscallRegs {
    uint64_t rbx, rip, rdx, rbp, rsi, rdi, r8, r9, r10, rflags, r12, r13, r14, r15, cr3, rax;
};
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
extern "C" void syscallHandler(SyscallRegs* regs) {
    printRegs(regs);
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
    nextProc();
}
}; // namespace task