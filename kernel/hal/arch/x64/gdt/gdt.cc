/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/hal/arch/x64/gdt/gdt.h>
#include <common/dbg/dbg.h>
#define GDT_ACCESS_ACCESSED             1 << 0
#define GDT_ACCESS_RW                   1 << 1
#define GDT_ACCESS_DIRECTION            1 << 2
#define GDT_ACCESS_EXECUTABLE           1 << 3
#define GDT_ACCESS_REGULAR_SEGMENT      1 << 4
#define GDT_ACCESS_DPL(level)           ((level & 0b11) << 5)
#define GDT_ACCESS_PRESENT              1 << 7

#define GDT_FLAGS_LONG          1 << 1
#define GDT_FLAG_SIZE           1 << 2
#define GDT_FLAG_GRANULARITY    1 << 3

#define SEGMENT(level) (level * 8)

#define DPL0_CS SEGMENT(1)
#define DPL0_DS SEGMENT(2)

#define DPL3_CS SEGMENT(7)
#define DPL3_DS SEGMENT(8)

#define GDT_ENTRY(access, flags) { \
    0,                             /* limit0 */ \
    0,                             /* base0 */ \
    0,                             /* base1 */ \
    access,                        /* access */ \
    0,                             /* limit1 */ \
    flags,                         /* flags */ \
    0                              /* base2 */ \
}

namespace hal::arch::x64::gdt{
    static GDTEntry entries[] = {
        GDT_ENTRY(0, 0),
        GDT_ENTRY(GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_DPL(0), GDT_FLAGS_LONG | GDT_FLAG_GRANULARITY), // Kernel 64 bit code
        GDT_ENTRY(GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW | GDT_ACCESS_DPL(0), GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE),                          // Kernel 64 bit data
        GDT_ENTRY(GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_DPL(3), GDT_FLAGS_LONG | GDT_FLAG_GRANULARITY), // User 64 bit code
        GDT_ENTRY(GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW | GDT_ACCESS_DPL(3), GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE),                          // User 64 bit data
    };
    extern "C" void loadGDTandReload(GDT* gdt);
    void init(){
        GDT gdt = {
            .limit = sizeof(entries)-1,
            .base = (uint64_t)entries
        };
        loadGDTandReload(&gdt);
    }
};