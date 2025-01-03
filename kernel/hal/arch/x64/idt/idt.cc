/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/hal/arch/x64/idt/idt.h>
#include <kernel/hal/arch/x64/idt/isr.h>
#include <common/io/regs.h>
#include <common/io/io.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#define KERNEL_ADDRESS 0xffffffff80000000

static const char* const exceptions[] = {
    "Divide by zero error",
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
    ""
};

namespace hal::arch::x64::idt{
    static IDTEntry entries[256];
    extern "C" void loadIDT(IDT* idt);
    void init(){
        IDT idt = {
            .limit = sizeof(entries)-1,
            .base = (uint64_t)entries
        };
        loadIDT(&idt);
        initGates();
        for(uint8_t i = 0; i < 255; ++i){
            enableGate(i);
        }
    }
    void registerHandler(uint8_t gate, void* function, uint8_t type){
        entries[gate] = IDT_ENTRY((uint64_t)function, 0x8, type, 0, 0);
    }
    void enableGate(uint8_t gate){
        entries[gate].present = 1;
    }
    void printRegs(io::Registers* regs){
        dbg::printf("\tv=0x%016.16llx e=0x%016.16llx\n", regs->interrupt_number, regs->error_code);
        dbg::printf("RAX=0x%016.16llx RBX=0x%016.16llx RCX=0x%016.16llx RDX=0x%016.16llx\n", regs->rax, regs->rbx, regs->rcx, regs->rdx);
        dbg::printf("RSI=0x%016.16llx RDI=0x%016.16llx RBP=0x%016.16llx RSP=0x%016.16llx\n", regs->rsi, regs->rdi, regs->rbp, regs->orig_rsp);
        dbg::printf("R8 =0x%016.16llx R9 =0x%016.16llx R10=0x%016.16llx R11=0x%016.16llx\n", regs->r8, regs->r9, regs->r10, regs->r11);
        dbg::printf("R12=0x%016.16llx R13=0x%016.16llx R14=0x%016.16llx R15=0x%016.16llx\n", regs->r12, regs->r13, regs->r14, regs->r15);
        dbg::printf("RIP=0x%016.16llx RFL=0x%016.16llx\n", regs->rip, regs->rflags);
        dbg::printf("ES =0x%016.16llx\n", regs->es);
        dbg::printf("CS =0x%016.16llx\n", regs->cs);
        dbg::printf("SS =0x%016.16llx\n", regs->ss);
        dbg::printf("DS =0x%016.16llx\n", regs->ds);
        dbg::printf("FS =0x%016.16llx\n", regs->fs);
        dbg::printf("GS =0x%016.16llx\n", regs->gs);
        dbg::printf("CR2=0x%016.16llx CR3=0x%016.16llx\n", io::rcr2(), regs->cr3);
    }
    extern "C" uint64_t handleInt(io::Registers* regs){
        printRegs(regs);
        if(regs->interrupt_number < 0x20){
            dbg::printf("Interrupt type: %s\n", exceptions[regs->interrupt_number]);
        }
        if(regs->rip >= KERNEL_ADDRESS){
            dbg::printf("TODO: Handle kernel interrupts (exit or throw a BSOD type) (rip = %016.16llx)\n", regs->rip);
            std::abort();
        }
        dbg::printf("TODO: Handle userland interrupts (send SIGSEGV) (rip = %016.16llx)\n", regs->rip);
        std::abort();
    }
};