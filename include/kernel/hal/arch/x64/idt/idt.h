/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_HAL_ARCH_X64_IDT_IDT_H_)
#define _KERNEL_HAL_ARCH_X64_IDT_IDT_H_
#include <cstdint>

namespace hal::arch::x64::idt{
    struct IDTEntry{
        uint16_t offset0;
        uint16_t segment_sel;
        uint8_t ist : 3;
        uint8_t reserved0 : 5;
        uint8_t gate_type : 4;
        uint8_t zero : 1;
        uint8_t dpl : 2;
        uint8_t present : 1;
        uint16_t offset1;
        uint32_t offset2;
        uint32_t reserved1;
    } __attribute__((packed));

    #define IDT_GATE_TYPE_INTERRUPT 0xE
    #define IDT_GATE_TYPE_TRAP_GATE 0xF

    #define IDT_ENTRY(offset, segment, type, dpl, ist) {                     \
        (uint16_t)((offset) & 0xFFFF),                 /* offset0 */        \
        (segment),                                     /* segment_sel */    \
        (ist),                                         /* ist */            \
        0,                                             /* reserved0 */      \
        (type),                                        /* gate_type */      \
        0,                                             /* zero */           \
        (dpl),                                         /* dpl */            \
        1,                                             /* present */        \
        (uint16_t)(((offset) >> 16) & 0xFFFF),         /* offset1 */        \
        (uint32_t)(((offset) >> 32) & 0xFFFFFFFF),     /* offset2 */        \
        0                                              /* reserved1 */      \
    }
    void init();
    void enableGate(uint8_t gate);
    void disablePageFaultProtection();
    void enablePageFaultProtection();
    void disableUDProtection();
    void enableUDProtection();
    void disableBPProtection();
    void enableBPProtection();
    void disableGPProtection();
    void enableGPProtection();
    void registerHandler(uint8_t gate, void* function, uint8_t type);
};

#endif // _KERNEL_HAL_ARCH_X64_IDT_IDT_H_
