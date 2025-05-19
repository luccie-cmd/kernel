#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstring>
#include <kernel/hal/arch/x64/gdt/gdt.h>
#include <kernel/mmu/mmu.h>
#include <kernel/task/task.h>
#define MODULE "Task manager"

uint64_t __attribute__((section(".trampoline.data")))  tempValue;
uint64_t* __attribute__((section(".trampoline.data"))) tempStack;
namespace task {
bool            initialized = false;
Process*        currentProc;
pid_t           pids = 0;
extern "C" void syscallEntry();
#define IDLE_ENTRY_POINT 0x802000
uint8_t idleCodeBuffer[] = {0xCC, 0x0F, 0x05, 0xEB, 0xFC};
void    initialize() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    currentProc      = new Process;
    initialized      = true;
    pids             = 10;
    pid_t        pid = getNewPID();
    uint64_t     newCR3 = reinterpret_cast<uint64_t>(mmu::vmm::getPML4(pid)) - mmu::vmm::getHHDM();
    uint64_t     codeSize   = sizeof(idleCodeBuffer);
    const size_t stack_size = 4 * PAGE_SIZE;
    uint64_t     stackPhys  = mmu::pmm::allocVirtual(stack_size);
    uint64_t     stackVirt  = USER_STACK_TOP - stack_size;
    for (size_t i = 0; i < stack_size; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), stackPhys + i, stackVirt + i, PROTECTION_RW,
                             MAP_PRESENT);
    }
    uint64_t codePhys = mmu::pmm::allocVirtual(codeSize);
    uint64_t codeVirt = IDLE_ENTRY_POINT;
    for (size_t i = 0; i < codeSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), codePhys + i, codeVirt + i, PROTECTION_RW,
                             MAP_PRESENT);
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), codePhys + i, codeVirt + i,
                             PROTECTION_RW | PROTECTION_KERNEL, MAP_PRESENT);
    }
    std::memcpy(reinterpret_cast<void*>(IDLE_ENTRY_POINT), idleCodeBuffer, codeSize);
    currentProc->pid = pid;
    std::memset(currentProc->fpuState, 0, sizeof(currentProc->fpuState));
    currentProc->registers = new io::Registers;
    mmu::vmm::mapPage(mmu::vmm::getPML4(pid),
                         mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID),
                                                   (uint64_t)currentProc->registers, false),
                         (uint64_t)currentProc->registers, PROTECTION_RW, MAP_PRESENT);
    currentProc->registers->cr3      = newCR3;
    currentProc->registers->orig_rsp = stackVirt + stack_size - 16;
    currentProc->registers->rbp      = currentProc->registers->orig_rsp;
    currentProc->registers->rip      = IDLE_ENTRY_POINT;
    currentProc->registers->rflags   = 0x202;
    currentProc->registers->ss       = 0x1B;
    currentProc->registers->cs       = 0x23;
    currentProc->next                = currentProc;
    currentProc->hasStarted          = false;
    io::wrmsr(0xC0000082, (uint64_t)syscallEntry);
    dbg::popTrace();
}
bool isInitialized() {
    return initialized;
}
pid_t getCurrentPID() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    dbg::popTrace();
    return currentProc->pid;
}
pid_t getNewPID() {
    if (!isInitialized()) {
        initialize();
    }
    return pids++;
}
void makeNewProcess(pid_t pid, uint8_t* codeBuffer, uint64_t codeSize, uint64_t entryPoint) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    uint64_t     newCR3 = reinterpret_cast<uint64_t>(mmu::vmm::getPML4(pid)) - mmu::vmm::getHHDM();
    const size_t stack_size = 4 * PAGE_SIZE;
    uint64_t     stackPhys  = mmu::pmm::allocVirtual(stack_size);
    uint64_t     stackVirt  = USER_STACK_TOP - stack_size;
    for (size_t i = 0; i < stack_size; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), stackPhys + i, stackVirt + i, PROTECTION_RW,
                          MAP_PRESENT);
    }
    uint64_t codePhys = mmu::pmm::allocVirtual(codeSize);
    uint64_t codeVirt = entryPoint;
    for (size_t i = 0; i < codeSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), codePhys + i, codeVirt + i, PROTECTION_RW,
                          MAP_PRESENT);
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), codePhys + i, codeVirt + i,
                          PROTECTION_RW | PROTECTION_KERNEL, MAP_PRESENT);
    }
    std::memcpy(reinterpret_cast<void*>(entryPoint), codeBuffer, codeSize);
    Process* proc = new Process;
    proc->pid     = pid;
    std::memset(proc->fpuState, 0, sizeof(proc->fpuState));
    proc->registers = new io::Registers;
    mmu::vmm::mapPage(
        mmu::vmm::getPML4(pid),
        mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)proc->registers, false),
        (uint64_t)proc->registers, PROTECTION_RW, MAP_PRESENT);
    proc->registers->cr3      = newCR3;
    proc->registers->orig_rsp = stackVirt + stack_size - 16;
    proc->registers->rbp      = proc->registers->orig_rsp;
    proc->registers->rip      = entryPoint;
    proc->registers->rflags   = 0x202;
    proc->registers->ss       = 0x1B;
    proc->registers->cs       = 0x23;
    proc->hasStarted          = false;
    if (currentProc->next == currentProc) {
        proc->next        = currentProc; // New process points to idle
        currentProc->next = proc;        // Idle points to new process
    } else {
        proc->next        = currentProc->next; // New process points to old first real process
        currentProc->next = proc;              // Idle points to new process
    }
    dbg::popTrace();
}
extern "C" void     switchProc(io::Registers* regs);
extern "C" uint64_t __trampoline_text_start;
extern "C" uint64_t __trampoline_text_end;
extern "C" uint64_t __trampoline_data_start;
extern "C" uint64_t __trampoline_data_end;
void                nextProc() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    currentProc = currentProc->next;
    if (currentProc->hasStarted == false) {
        currentProc->hasStarted = true;
        uint64_t trampoline_va  = (uint64_t)&__trampoline_text_start;
        uint64_t trampoline_phys =
            mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        while (trampoline_va < (uint64_t)&__trampoline_text_end) {
            mmu::vmm::mapPage(mmu::vmm::getPML4(currentProc->pid), trampoline_phys, trampoline_va,
                                             PROTECTION_RW, MAP_PRESENT);
            trampoline_va += PAGE_SIZE;
            trampoline_phys =
                mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        }
        trampoline_va = (uint64_t)&__trampoline_data_start;
        trampoline_phys =
            mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        while (trampoline_va < (uint64_t)&__trampoline_data_end) {
            mmu::vmm::mapPage(mmu::vmm::getPML4(currentProc->pid), trampoline_phys, trampoline_va,
                                             PROTECTION_RW, MAP_PRESENT);
            trampoline_va += PAGE_SIZE;
            trampoline_phys =
                mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
        }
        // dbg::printf("0x%llx\n", (uint64_t)tempStack & ~(0xFFF));
        // for (size_t i = 0; i < 3; ++i) {
        //     mmu::vmm::mapPage(mmu::vmm::getPML4(currentProc->pid),
        //                                      ((uint64_t)tempStack & ~(0xFFF)) + i * PAGE_SIZE,
        //                                      ((uint64_t)tempStack & ~(0xFFF)) + i * PAGE_SIZE,
        //                                      PROTECTION_RW | PROTECTION_NOEXEC, MAP_PRESENT);
        // }
        hal::arch::x64::gdt::setRSP0(currentProc->pid);
    }
    if ((mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(currentProc->pid),
                                                  (uint64_t)switchProc & (~0xFFF), true)) == 0) {
        dbg::printm(MODULE, "switchProc(io::Registers*) became unmapped!!!\n");
        std::abort();
    }
    dbg::popTrace();
    switchProc(currentProc->registers);
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
    // printRegs(regs);
    currentProc->registers->rax      = regs->rax;
    currentProc->registers->rbx      = regs->rbx;
    currentProc->registers->rip      = regs->rip;
    currentProc->registers->rdx      = regs->rdx;
    currentProc->registers->rsi      = regs->rsi;
    currentProc->registers->rdi      = regs->rdi;
    currentProc->registers->rbp      = regs->rbp;
    currentProc->registers->orig_rsp = tempValue;
    currentProc->registers->r8       = regs->r8;
    currentProc->registers->r9       = regs->r9;
    currentProc->registers->r10      = regs->r10;
    currentProc->registers->r12      = regs->r12;
    currentProc->registers->r13      = regs->r13;
    currentProc->registers->r14      = regs->r14;
    currentProc->registers->r15      = regs->r15;
    currentProc->registers->rflags   = regs->rflags;
    currentProc->registers->cr3      = regs->cr3;
    nextProc();
    // std::abort();
}
}; // namespace task