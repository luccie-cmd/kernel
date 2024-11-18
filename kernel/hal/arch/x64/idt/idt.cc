/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/hal/arch/x64/idt/idt.h>
#include <kernel/hal/arch/x64/idt/isr.h>
#include <common/io/regs.h>
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
    extern "C" uint64_t handleInt(io::Registers* regs){
        if(regs->interrupt_number < 0x20){
            dbg::printf("Interrupt type: %s\n", exceptions[regs->interrupt_number]);
        }
        if(regs->rip >= KERNEL_ADDRESS){
            dbg::printf("TODO: Handle kernel interrupts (exit or throw a BSOD type) (rip = %llx)\n", regs->rip);
            std::abort();
        }
        dbg::printf("TODO: Handle userland interrupts (send SIGSEGV) (rip = %llx)\n", regs->rip);
        std::abort();
    }
};