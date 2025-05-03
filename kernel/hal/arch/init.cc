/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/hal/arch/init.h>
#include <kernel/hal/arch/x64/gdt/gdt.h>
#include <kernel/hal/arch/x64/idt/idt.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstring>

extern uint64_t *__bss_start;
extern uint64_t *__bss_end;

namespace hal::arch{
    extern "C" void initX64();
    void earlyInit(){
        io::cli();
        std::memset(__bss_start, 0, (uint64_t)(__bss_end)-(uint64_t)(__bss_start));
        x64::idt::init();
        x64::gdt::init();
        initX64();
        io::sti();
    }
    void midInit(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        io::cli();
        x64::irq::init();
        io::sti();
        dbg::popTrace();
    }
};