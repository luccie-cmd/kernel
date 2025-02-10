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
    static void __zeroBSS(){
        std::memset(&__bss_start, 0, &__bss_end - &__bss_start);
    }
    static void __initSSE(){
        __asm__ volatile ( "clts" );
	    size_t t;
	    __asm__ volatile ( "mov %%cr0, %0" : "=r"(t) );
	    t &= ~(1 << 2);
	    t |= (1 << 1);
	    __asm__ volatile ( "mov %0, %%cr0" :: "r"(t) );
	    __asm__ volatile ( "mov %%cr4, %0" : "=r"(t) );
	    t |= 3 << 9;
	    __asm__ volatile ( "mov %0, %%cr4" :: "r"(t) );
    }
    void earlyInit(){
        io::cli();
        // __zeroBSS();
        x64::idt::init();
        x64::gdt::init();
        __initSSE();
        io::sti();
    }
};