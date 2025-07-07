/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstring>
#include <kernel/hal/arch/init.h>
#include <kernel/hal/arch/x64/gdt/gdt.h>
#include <kernel/hal/arch/x64/idt/idt.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#define MSR_STAR 0xC0000081
#define MSR_LSTAR 0xC0000082
#define MSR_CSTAR 0xC0000083
#define MSR_SYSCALL_MASK 0xC0000084

extern uint64_t __bss_start[];
extern uint64_t __bss_end[];

namespace hal::arch {
extern "C" void initX64();
void            earlyInit() {
    initX64();
    io::cli();
    std::memset(__bss_start, 0, __bss_end - __bss_start);
    x64::idt::init();
    x64::gdt::init();
    io::sti();
}
void midInit() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    io::cli();
    x64::irq::init();
    io::sti();
    dbg::printf("Setting LSTAR\n");
    uint64_t star_msr = ((uint64_t)(0x13) << 48) | ((uint64_t)0x08 << 32);
    io::wrmsr(MSR_STAR, star_msr);
    io::wrmsr(MSR_SYSCALL_MASK, 0x202);
    hal::arch::x64::gdt::setRSP0(KERNEL_PID);
    dbg::popTrace();
}
}; // namespace hal::arch