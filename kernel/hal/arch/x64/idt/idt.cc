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
        if(regs->rip >= KERNEL_ADDRESS){
            dbg::print("TODO: Handle kernel interrupts (exit probably)\n");
            std::abort();
        }
        dbg::print("TODO: Handle userladn interrupts (send SIGSEGV)\n");
        std::abort();
    }
};