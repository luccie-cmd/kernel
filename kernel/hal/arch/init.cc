/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/hal/arch/init.h>
#include <kernel/hal/arch/x64/gdt/gdt.h>
#include <kernel/hal/arch/x64/idt/idt.h>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstring>

extern uint64_t *__bss_start;
extern uint64_t *__bss_end;

namespace hal::arch{
    static void zeroBSS(){
        std::memset(&__bss_start, 0, &__bss_end - &__bss_start);
    }
    void earlyInit(){
        io::cli();
        zeroBSS();
        x64::idt::init();
        x64::gdt::init();
        io::sti();
    }
};