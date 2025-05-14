/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <common/dbg/dbg.h>
#include <kernel/hal/arch/x64/gdt/gdt.h>
#include <kernel/mmu/mmu.h>
#define GDT_ACCESS_ACCESSED 1 << 0
#define GDT_ACCESS_RW 1 << 1
#define GDT_ACCESS_DIRECTION 1 << 2
#define GDT_ACCESS_EXECUTABLE 1 << 3
#define GDT_ACCESS_REGULAR_SEGMENT 1 << 4
#define GDT_ACCESS_DPL(level) ((level & 0b11) << 5)
#define GDT_ACCESS_PRESENT 1 << 7

#define GDT_FLAGS_LONG 1 << 1
#define GDT_FLAG_SIZE 1 << 2
#define GDT_FLAG_GRANULARITY 1 << 3

#define SEGMENT(level) (level * 8)

#define GDT_ENTRY(access, flags)                                                                   \
    {                                                                                              \
        0,      /* limit0 */                                                                       \
        0,      /* base0 */                                                                        \
        0,      /* base1 */                                                                        \
        access, /* access */                                                                       \
        0,      /* limit1 */                                                                       \
        flags,  /* flags */                                                                        \
        0       /* base2 */                                                                        \
    }

hal::arch::x64::gdt::TSS tss __attribute__((aligned(4096), section(".trampoline.data")));
namespace hal::arch::x64::gdt {
static GDTEntry __attribute__((section(".trampoline.data"))) entries[] = {
    GDT_ENTRY(0, 0),
    GDT_ENTRY(GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW |
                  GDT_ACCESS_EXECUTABLE | GDT_ACCESS_DPL(0),
              GDT_FLAGS_LONG | GDT_FLAG_GRANULARITY), // Kernel 64 bit code
    GDT_ENTRY(GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW | GDT_ACCESS_DPL(0),
              GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE), // Kernel 64 bit data
    GDT_ENTRY(GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW |
                  GDT_ACCESS_EXECUTABLE | GDT_ACCESS_DPL(3),
              GDT_FLAGS_LONG | GDT_FLAG_GRANULARITY), // User 64 bit code
    GDT_ENTRY(GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW | GDT_ACCESS_DPL(3),
              GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE), // User 64 bit data
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
};
void initTSS() {
    uint32_t tss_limit = sizeof(TSS) - 1;
    uint64_t tss_base  = reinterpret_cast<uint64_t>(&tss);
    entries[5]         = {
                .limit_low    = static_cast<uint16_t>(tss_limit & 0xFFFF),
                .base_low     = static_cast<uint16_t>(tss_base & 0xFFFF),
                .base_middle  = static_cast<uint8_t>((tss_base >> 16) & 0xFF),
                .access       = 0x8B,
                .limit_middle = 0,
                .flags        = 0,
                .base_high    = static_cast<uint8_t>((tss_base >> 24) & 0xFF),
    };
    entries[6] = {
        .limit_low    = static_cast<uint16_t>(tss_base >> 32),
        .base_low     = static_cast<uint16_t>(tss_base >> 48),
        .base_middle  = 0,
        .access       = 0,
        .limit_middle = 0,
        .flags        = 0,
        .base_high    = 0,
    };
    tss.iomap_base = sizeof(TSS);
}
extern "C" void loadGDT(uint64_t base, uint16_t limit);
void            init() {
    initTSS();
    loadGDT((uint64_t)entries, sizeof(entries) - 1);
    dbg::printf("0b%llb 0b%llb 0b%llb 0b%llb\n",
                           GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW |
                               GDT_ACCESS_EXECUTABLE | GDT_ACCESS_DPL(0),
                           GDT_FLAGS_LONG | GDT_FLAG_GRANULARITY,
                           GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW | GDT_ACCESS_DPL(0),
                           GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE);
    dbg::printf("0b%llb 0b%llb 0b%llb 0b%llb\n",
                           GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW |
                               GDT_ACCESS_EXECUTABLE | GDT_ACCESS_DPL(3),
                           GDT_FLAGS_LONG | GDT_FLAG_GRANULARITY,
                           GDT_ACCESS_PRESENT | GDT_ACCESS_REGULAR_SEGMENT | GDT_ACCESS_RW | GDT_ACCESS_DPL(3),
                           GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE);
}
void setRSP0(task::pid_t pid) {
    uint64_t stackPhysical    = mmu::pmm::allocate();
    uint64_t ISTstackPhysical = mmu::pmm::allocate();
    dbg::printf("Set TSS.RSP0 to physical address 0x%llx\n", tss.rsp0);
    dbg::printf("Set TSS.IST1 to physical address 0x%llx\n", tss.ist1);
    tss.rsp0 = stackPhysical + mmu::vmm::getHHDM() + PAGE_SIZE - 16;
    tss.ist1 = ISTstackPhysical + mmu::vmm::getHHDM() + PAGE_SIZE - 16;
    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), stackPhysical, stackPhysical + mmu::vmm::getHHDM(),
                      PROTECTION_RW, MAP_GLOBAL | MAP_PRESENT);
    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), ISTstackPhysical,
                      ISTstackPhysical + mmu::vmm::getHHDM(), PROTECTION_RW,
                      MAP_GLOBAL | MAP_PRESENT);
}
}; // namespace hal::arch::x64::gdt

// global halTssInit
// extern GDT
// extern abort
// section .text
// halTssInit:
//     push rdi
//     push rsi
//     lea rax, [tssEntry]
//     mov rdi, TSS_END
//     mov rsi, TSS
//     sub rdi, rsi
//     dec rdi
//     mov [rax+0], di
//     mov [rax+2], si
//     shr rsi, 16
//     mov [rax+4], sil
//     mov BYTE [rax+5], 0x89
//     mov BYTE [rax+6], 0
//     shr rsi, 8
//     mov [rax+7], si
//     shr rsi, 8
//     mov [rax+8], esi
//     mov DWORD [rax+12], 0
//     mov rax, [tssEntry]
//     mov [GDT+0x28], rax
//     mov rax, [tssEntry+8]
//     mov [GDT+0x30], rax
//     pop rsi
//     pop rdi
//     ret

// section .data
// global TSS
// TSS:
//     .reserved1: dd 0
//     .rsp0:      dq 0
//     .rsp1:      dq 0
//     .rsp2:      dq 0
//     .reserved2: dq 0
//     .ist1:      dq 0
//     .ist2:      dq 0
//     .ist3:      dq 0
//     .ist4:      dq 0
//     .ist5:      dq 0
//     .ist6:      dq 0
//     .ist7:      dq 0
//     .reserved3: dq 0
//     .reserved4: dw 0
//     .iopb:      dw 104
// TSS_END:
// tssEntry: times 2 dq 0