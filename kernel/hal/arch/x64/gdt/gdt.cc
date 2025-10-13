/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/spinlock.h>
#include <cstdlib>
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

hal::arch::x64::gdt::TSS* tss = nullptr;
namespace hal::arch::x64::gdt {
void initTSS() {
    tss->iomap_base = sizeof(TSS);
}
void setRSP0() {
    uint64_t stack1Physical   = mmu::pmm::allocate();
    uint64_t ISTstackPhysical = mmu::pmm::allocate();
    tss->rsp0                 = stack1Physical + mmu::vmm::getHHDM() + PAGE_SIZE - 16;
    tss->ist1                 = ISTstackPhysical + mmu::vmm::getHHDM() + PAGE_SIZE - 16;
    dbg::printf("Set TSS->RSP0 to physical address 0x%llx\n", tss->rsp0);
    dbg::printf("Set TSS->IST1 to physical address 0x%llx\n", tss->ist1);
}
std::Spinlock initLock;
struct GDTR {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));
extern "C" void loadGDT(GDTR* gdtr);
void            init() {
    initLock.lock();
    GDTEntry* entries =
        (GDTEntry*)(mmu::pmm::allocVirtual(7 * sizeof(GDTEntry)) + mmu::vmm::getHHDM());
    tss                = (TSS*)(mmu::pmm::allocate() + mmu::vmm::getHHDM());
    uint32_t tss_limit = sizeof(TSS) - 1;
    uint64_t tss_base  = reinterpret_cast<uint64_t>(tss);
    entries[0]         = GDT_ENTRY(0, 0);
    entries[1]         = GDT_ENTRY(0x9A, 0xa); // Kernel 64 bit code
    entries[2]         = GDT_ENTRY(0x93, 0xc); // Kernel 64 bit data
    entries[3]         = GDT_ENTRY(0xFA, 0xa); // User 64 bit code
    entries[4]         = GDT_ENTRY(0xF3, 0xc); // User 64 bit data
    entries[5]         = {
                           .limit_low    = static_cast<uint16_t>(tss_limit & 0xFFFF),
                           .base_low     = static_cast<uint16_t>(tss_base & 0xFFFF),
                           .base_middle  = static_cast<uint8_t>((tss_base >> 16) & 0xFF),
                           .access       = 0x89,
                           .limit_middle = static_cast<uint8_t>((tss_limit >> 16) & 0xF),
                           .flags        = 0,
                           .base_high    = static_cast<uint8_t>((tss_base >> 24) & 0xFF),
    };

    entries[6] = {
                   .limit_low    = static_cast<uint16_t>((tss_base >> 32) & 0xFFFF),
                   .base_low     = static_cast<uint16_t>((tss_base >> 48) & 0xFFFF),
                   .base_middle  = 0,
                   .access       = 0,
                   .limit_middle = 0,
                   .flags        = 0,
                   .base_high    = 0,
    };
    GDTR* gdt  = (GDTR*)(mmu::pmm::allocate() + mmu::vmm::getHHDM());
    gdt->base  = (uint64_t)entries;
    gdt->limit = 7 * sizeof(GDTEntry) - 1;
    initTSS();
    loadGDT(gdt);
    setRSP0();
    mapStacksToProc(KERNEL_PID, mmu::vmm::getPML4(KERNEL_PID));
    initLock.unlock();
}
void mapStacksToProc(task::pid_t pid, mmu::vmm::PML4* pml4) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    GDTR     gdtr;
    uint16_t tr;
    __asm__ volatile("sgdt %0" : "=m"(gdtr));
    __asm__ volatile("str %0" : "=a"(tr));
    GDTEntry* tssEntryLow  = (GDTEntry*)((uint64_t*)(gdtr.base + tr));
    GDTEntry* tssEntryHigh = (GDTEntry*)((uint64_t*)(gdtr.base + tr + 8));
    GDTEntry* userCS       = (GDTEntry*)(gdtr.base + 0x18);
    GDTEntry* userDS       = (GDTEntry*)(gdtr.base + 0x20);
    GDTEntry* kernelCS     = (GDTEntry*)(gdtr.base + 0x08);
    GDTEntry* kernelDS     = (GDTEntry*)(gdtr.base + 0x10);
    dbg::printf("Kernel CS access, flags = %hx %hx\n", kernelCS->access, kernelCS->flags);
    dbg::printf("Kernel DS access, flags = %hx %hx\n", kernelDS->access, kernelDS->flags);
    dbg::printf("User CS access, flags = %hx %hx\n", userCS->access, userCS->flags);
    dbg::printf("User DS access, flags = %hx %hx\n", userDS->access, userDS->flags);
    dbg::printf("Mapping proc %llu (Checking: %s)\n", pid, pid != KERNEL_PID ? "true" : "false");
    uint64_t tssValue =
        ((uint64_t)tssEntryHigh->base_low << 48) | ((uint64_t)tssEntryHigh->limit_low << 32) |
        ((uint64_t)tssEntryLow->base_high << 24) | ((uint64_t)tssEntryLow->base_middle << 16) |
        ((uint64_t)tssEntryLow->base_low);
    TSS* tempTss = (TSS*)(tssValue);
    if (pid != KERNEL_PID) {
        if (mmu::vmm::getPhysicalAddr(pml4, (uint64_t)tempTss, false, false) == 0) {
            mmu::vmm::mapPage(pml4, ((uint64_t)tempTss) - mmu::vmm::getHHDM(), (uint64_t)tempTss,
                              PROTECTION_RW | PROTECTION_KERNEL, MAP_PRESENT);
        }
        dbg::printf("Mapped GDTR.BASE: 0x%llx Actual GDTR.BASE: 0x%llx\n",
                    mmu::vmm::getPhysicalAddr(pml4, gdtr.base, false, false), gdtr.base);
        if (mmu::vmm::getPhysicalAddr(pml4, gdtr.base, false, false) == 0) {
            mmu::vmm::mapPage(pml4, gdtr.base - mmu::vmm::getHHDM(), gdtr.base,
                              PROTECTION_RW | PROTECTION_KERNEL, MAP_PRESENT);
        }
        if (mmu::vmm::getPhysicalAddr(pml4, tempTss->rsp0 - PAGE_SIZE + 16, false, false) == 0) {
            mmu::vmm::mapPage(pml4, tempTss->rsp0 - mmu::vmm::getHHDM() - PAGE_SIZE + 16,
                              tempTss->rsp0 - PAGE_SIZE + 16,
                              PROTECTION_RW | PROTECTION_KERNEL | PROTECTION_NOEXEC, MAP_PRESENT);
        }
        if (mmu::vmm::getPhysicalAddr(pml4, tempTss->ist1 - PAGE_SIZE + 16, false, false) == 0) {
            mmu::vmm::mapPage(pml4, tempTss->ist1 - mmu::vmm::getHHDM() - PAGE_SIZE + 16,
                              tempTss->ist1 - PAGE_SIZE + 16,
                              PROTECTION_RW | PROTECTION_KERNEL | PROTECTION_NOEXEC, MAP_PRESENT);
        }
    }
    dbg::popTrace();
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