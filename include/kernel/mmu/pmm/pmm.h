/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_MMU_PMM_PMM_H_)
#define _KERNEL_MMU_PMM_PMM_H_
#include <cstddef>
#include <cstdint>

namespace mmu{
namespace pmm{
    struct node{
        size_t size;
        node* next;
    };
    void initialize();
    bool isInitialized();
    uint64_t allocate();
    uint64_t allocVirtual(uint64_t size);
    void free(uint64_t ptr, uint64_t size);
    void printInfo();
};
};

#endif // _KERNEL_MMU_PMM_PMM_H_
