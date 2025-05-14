/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_HAL_ARCH_X64_GDT_GDT_H_)
#define _KERNEL_HAL_ARCH_X64_GDT_GDT_H_
#include <cstdint>
#include <kernel/task/types.h>

namespace hal::arch::x64::gdt {
struct TSS {
    uint32_t reserved0;
    uint64_t rsp0; // Kernel stack pointer
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iomap_base;
} __attribute__((packed));
struct GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  limit_middle : 4;
    uint8_t  flags : 4;
    uint8_t  base_high : 8;
} __attribute__((packed));
void init();
void setRSP0(task::pid_t pid);
}; // namespace hal::arch::x64::gdt

#endif // _KERNEL_HAL_ARCH_X64_GDT_GDT_H_
