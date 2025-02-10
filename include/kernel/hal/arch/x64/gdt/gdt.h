/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_HAL_ARCH_X64_GDT_GDT_H_)
#define _KERNEL_HAL_ARCH_X64_GDT_GDT_H_
#include <cstdint>

namespace hal::arch::x64::gdt{
    struct GDTEntry{
        uint16_t limit_low;
        uint16_t base_low;
        uint8_t base_middle;
        uint8_t access;
        uint8_t limit_middle : 4;
        uint8_t flags : 4;
        uint8_t base_high : 8;
    } __attribute__((packed));
    void init();
};

#endif // _KERNEL_HAL_ARCH_X64_GDT_GDT_H_
