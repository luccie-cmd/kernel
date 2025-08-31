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
    struct __attribute__((packed)) node{
        uint64_t prePadding[5];
        size_t size;
        size_t freedSize;
        size_t allocSize;
        node* prev;
        node* next;
        uint8_t free;
        uint8_t padding[7];
        uint64_t postPadding[5];
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
