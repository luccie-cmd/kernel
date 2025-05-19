/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/io/regs.h>
#include <cstdlib>
#include <cstring>
#include <kernel/hal/arch/x64/idt/idt.h>
#include <kernel/hal/arch/x64/idt/isr.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <kernel/mmu/mmu.h>
#include <kernel/task/task.h>
#define KERNEL_ADDRESS 0xffffffff80000000
#define PAGE_MASK 0xfffffffffffff000

static const char* const exceptions[] = {"Divide by zero error",
                                         "Debug",
                                         "Non-maskable Interrupt",
                                         "Breakpoint",
                                         "Overflow",
                                         "Bound Range Exceeded",
                                         "Invalid Opcode",
                                         "Device Not Available",
                                         "Double Fault",
                                         "Coprocessor Segment Overrun",
                                         "Invalid TSS",
                                         "Segment Not Present",
                                         "Stack-Segment Fault",
                                         "General Protection Fault",
                                         "Page Fault",
                                         "",
                                         "x87 Floating-Point Exception",
                                         "Alignment Check",
                                         "Machine Check",
                                         "SIMD Floating-Point Exception",
                                         "Virtualization Exception",
                                         "Control Protection Exception ",
                                         "",
                                         "",
                                         "",
                                         "",
                                         "",
                                         "",
                                         "Hypervisor Injection Exception",
                                         "VMM Communication Exception",
                                         "Security Exception",
                                         ""};

typedef void (*ExceptionHandler)(io::Registers* regs);
ExceptionHandler exceptionHandlers[32];
namespace hal::arch::x64::idt {
static IDTEntry __attribute__((section(".trampoline.data"))) entries[256];
extern "C" void                                              loadIDT(uint64_t base, uint16_t limit);
void                                                         handlePF(io::Registers*);
void                                                         handleUD(io::Registers*);
void                                                         init() {
    loadIDT((uint64_t)entries, sizeof(entries) - 1);
    initGates();
    for (uint8_t i = 0; i < 255; ++i) {
        enableGate(i);
    }
    enablePageFaultProtection();
    enableUDProtection();
    std::memset(exceptionHandlers, 0, sizeof(exceptionHandlers) / sizeof(exceptionHandlers[0]));
}
void registerHandler(uint8_t gate, void* function, uint8_t type) {
    entries[gate] = IDT_ENTRY((uint64_t)function, 0x8, type, 3, 1);
}
void enableGate(uint8_t gate) {
    entries[gate].present = 1;
}
struct PageFaultError {
    uint8_t  PPV : 1;   // 1=PPV   0=NP
    uint8_t  write : 1; // 1=Write 0=Read
    uint8_t  user : 1;  // 1=CPL3  0=CPL0
    uint8_t  rsvw : 1;  // Reserved field write
    uint8_t  insF : 1;  // Instruction fetch
    uint8_t  PKV : 1;   // Protection key violation
    uint8_t  SS : 1;    // Shadow stack
    uint8_t  reserved0;
    uint8_t  SGX : 1; // Software guard extension
    uint16_t reserved1;
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
extern "C" void printRegs(io::Registers* regs) {
    dbg::printf("\tv=0x%016.16llx e=0x%016.16llx\n", regs->interrupt_number, regs->error_code);
    dbg::printf("RAX=0x%016.16llx RBX=0x%016.16llx RCX=0x%016.16llx RDX=0x%016.16llx\n", regs->rax,
                regs->rbx, regs->rcx, regs->rdx);
    dbg::printf("RSI=0x%016.16llx RDI=0x%016.16llx RBP=0x%016.16llx RSP=0x%016.16llx\n", regs->rsi,
                regs->rdi, regs->rbp, regs->orig_rsp);
    dbg::printf("R8 =0x%016.16llx R9 =0x%016.16llx R10=0x%016.16llx R11=0x%016.16llx\n", regs->r8,
                regs->r9, regs->r10, regs->r11);
    dbg::printf("R12=0x%016.16llx R13=0x%016.16llx R14=0x%016.16llx R15=0x%016.16llx\n", regs->r12,
                regs->r13, regs->r14, regs->r15);
    dbg::printf("RIP=0x%016.16llx RFL=", regs->rip);
    printRfl(regs->rflags);
    dbg::printf("CS =0x%02.2llx\n", regs->cs);
    dbg::printf("ES =0x%02.2llx\n", regs->es);
    dbg::printf("DS =0x%02.2llx\n", regs->ds);
    dbg::printf("FS =0x%02.2llx\n", regs->fs);
    dbg::printf("GS =0x%02.2llx\n", regs->gs);
    dbg::printf("SS =0x%02.2llx\n", regs->ss);
    dbg::printf("CR2=0x%016.16llx CR3=0x%016.16llx\n", io::rcr2(), regs->cr3);
}
extern "C" void handleInt(io::Registers* regs) {
    if (regs->interrupt_number < 0x20) {
        dbg::printf("Interrupt type: %s\n", exceptions[regs->interrupt_number]);
        printRegs(regs);
        if (exceptionHandlers[regs->interrupt_number]) {
            exceptionHandlers[regs->interrupt_number](regs);
        } else {
            dbg::printf("TODO: Add exception handler for %llu\n", regs->interrupt_number);
            // std::abort();
        }
        return;
    }
    if (regs->interrupt_number >= 0x20) {
        irq::handleInt(regs);
    }
}
void disablePageFaultProtection() {
    exceptionHandlers[0xE] = nullptr;
}
void enablePageFaultProtection() {
    exceptionHandlers[0xE] = handlePF;
}
void disableUDProtection() {
    exceptionHandlers[0x6] = nullptr;
}
void enableUDProtection() {
    exceptionHandlers[0x6] = handleUD;
}
static mmu::vmm::vmm_address getVMMfromVA(uint64_t vaddr) {
    mmu::vmm::vmm_address result;
    result.padding = (vaddr >> 48) & 0xFFFF;
    result.pml4e   = (vaddr >> 39) & 0x1FF;
    result.pdpe    = (vaddr >> 30) & 0x1FF;
    result.pde     = (vaddr >> 21) & 0x1FF;
    result.pte     = (vaddr >> 12) & 0x1FF;
    result.offset  = vaddr & 0xFFF;
    return result;
}
void handlePF(io::Registers* regs) {
    io::cli();
    disablePageFaultProtection();
    PageFaultError  err         = *(PageFaultError*)(&regs->error_code);
    mmu::vmm::PML4* pml4        = reinterpret_cast<mmu::vmm::PML4*>(regs->cr3);
    uint64_t        physicalCR2 = mmu::vmm::getPhysicalAddr(pml4, io::rcr2(), false);
    dbg::printf("Physical CR2 = 0x%llx\n", physicalCR2);
    dbg::printf("PPV: %hhu, "
                "write: %hhu, "
                "user: %hhu, "
                "rsvw: %hhu, "
                "insF: %hhu, "
                "PKV: %hhu, "
                "SS: %hhu\n",
                err.PPV, err.write, err.user, err.rsvw, err.insF, err.PKV, err.SS);
    mmu::vmm::vmm_address vma = getVMMfromVA(io::rcr2());
    mmu::vmm::PDPE*       pdpe =
        reinterpret_cast<mmu::vmm::PDPE*>(mmu::vmm::makeVirtual(pml4[vma.pml4e].pdpe_ptr << 12));
    mmu::vmm::PDE* pde =
        reinterpret_cast<mmu::vmm::PDE*>(mmu::vmm::makeVirtual(pdpe[vma.pdpe].pde_ptr << 12));
    mmu::vmm::PTE* pte =
        reinterpret_cast<mmu::vmm::PTE*>(mmu::vmm::makeVirtual(pde[vma.pde].pte_ptr << 12));
    dbg::printf("%hhu %hhu %hhu %hhu\n", pml4[vma.pml4e].no_execute, pdpe[vma.pdpe].no_execute,
                pde[vma.pde].no_execute, pte[vma.pte].no_execute);
    if (err.PPV == 0) {
        if (io::rcr2() == 0) {
            dbg::print("Cannot map a page at NULL\n");
            std::abort();
        }
        if (physicalCR2 == 0xDEADB000) {
            dbg::printf("On demand mapping of 0x%llx\n", io::rcr2());
            mmu::vmm::mapPage(pml4, mmu::pmm::allocate(), io::rcr2() & PAGE_MASK,
                              PROTECTION_RW |
                                  (task::getCurrentPID() == KERNEL_PID ? PROTECTION_KERNEL : 0),
                              MAP_PRESENT);
        } else {
            dbg::printf("TODO: Exit program as it has attempted to use an invalid address 0x%llx",
                        physicalCR2);
            std::abort();
        }
    } else {
        dbg::printf("TODO: Handle other types of page faults!!!\n");
        std::abort();
    }
    dbg::printf("Handled pagefault for address 0x%llx\n", io::rcr2() & PAGE_MASK);
    enablePageFaultProtection();
    io::sti();
}
void handleUD(io::Registers* regs) {
    disableUDProtection();
    uint8_t* instructionPtr = (uint8_t*)regs->rip;
    uint8_t  opcode         = *instructionPtr;
    dbg::printf("Opcode: 0x%x\n", opcode);
    std::abort();
    enableUDProtection();
}
}; // namespace hal::arch::x64::idt