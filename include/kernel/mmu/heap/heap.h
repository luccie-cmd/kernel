/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_MMU_HEAP_HEAP_H_)
#define _KERNEL_MMU_HEAP_HEAP_H_
#include <cstddef>
#include <cstdint>

namespace mmu{
namespace heap{
    struct node{
        size_t size;
        size_t freedSize;
        size_t allocSize;
        // size_t addedPadding;
        bool free;
        node* prev;
        node* next;
    };
    void initialize(uint64_t pmm_size, uint64_t vmm_max);
    bool isInitialized();
    void* allocate(size_t size);
    void* allocate(size_t size, size_t align);
    void free(void* ptr, size_t size, size_t align);
    void free(void* ptr, size_t size);
    void free(void* ptr);
    void printInfo();
};
};

#endif // _KERNEL_MMU_HEAP_HEAP_H_
