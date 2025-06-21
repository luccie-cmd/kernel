/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_MMU_VMM_VMM_H_)
#define _KERNEL_MMU_VMM_VMM_H_
#include <cstddef>
#include <kernel/task/types.h>
#include <cstdint>
#include "types.h"
#define PROTECTION_RW     (1 << 0)
#define PROTECTION_NOEXEC (1 << 1)
#define PROTECTION_KERNEL (1 << 2)
#define MAP_GLOBAL        (1 << 0)
#define MAP_PRESENT       (1 << 1)
#define MAP_UC            (1 << 2)
#define MAP_WT            (1 << 3)

namespace mmu{
namespace vmm{
    struct vmm_address{
        uint64_t padding : 16;
        uint64_t pml4e : 9;
        uint64_t pdpe : 9;
        uint64_t pde : 9;
        uint64_t pte : 9;
        uint64_t offset : 12;
    } __attribute__((packed));
    void initialize();
    bool isInitialized();
    PML4* getPML4(task::pid_t pid);
    void mapPage(PML4* pml4, size_t physicalAddr, size_t virtualAddr, int protFlags, int mapFlags);
    void unmapPage(PML4* pml4, size_t virtualAddr);
    void mapPage(size_t virtualAddr);
    uint64_t getPhysicalAddr(PML4* pml4, uint64_t addr, bool silent, bool ignorePresent=true);
    uint64_t makeVirtual(uint64_t addr);
    uint64_t getHHDM();
};
};

#endif // _KERNEL_MMU_VMM_VMM_H_
