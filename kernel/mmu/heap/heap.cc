/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <kernel/mmu/mmu.h>
#include <kernel/task/task.h>
#include <utility>
#include <vector>
#define MODULE "MMU HEAP"
#define PMM_SIZE MEGABYTE
#define VMM_MAX 2 * MEGABYTE
static constexpr uint64_t PADDING_PATTERN    = 0xBEBEBEBEBEBEBEBEULL;
static constexpr size_t   MINIMUM_ALIGN_SIZE = sizeof(mmu::heap::node);
#define PRINT_PADDING 1

namespace mmu::heap {
static bool     __initialized = false;
static node*    __head        = nullptr;
static uint64_t __pmmSize, __vmmMax;
void            initialize(uint64_t pmm_size, uint64_t vmm_max) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    __pmmSize     = pmm_size;
    __vmmMax      = vmm_max;
    uint64_t base = pmm::allocVirtual(__pmmSize) + vmm::getHHDM();
    __head        = (node*)base;
    memset(__head->prePadding, PADDING_PATTERN & 0xFF, sizeof(__head->prePadding));
    memset(__head->postPadding, PADDING_PATTERN & 0xFF, sizeof(__head->postPadding));
    __head->free      = true;
    __head->freedSize = 0;
    __head->allocSize = pmm_size - sizeof(node);
    __head->size      = pmm_size - sizeof(node);
    __head->next      = nullptr;
    __head->prev      = nullptr;
    __initialized     = true;
    dbg::popTrace();
}
static bool validateNode(const node* n) {
    for (unsigned i = 0; i < 5; ++i) {
        if (n->prePadding[i] != PADDING_PATTERN) {
            return false;
        }
    }
    for (unsigned i = 0; i < 5; ++i) {
        if (n->postPadding[i] != PADDING_PATTERN) {
            return false;
        }
    }
    if (n->allocSize > n->size || n->freedSize > n->allocSize) {
        return false;
    }
    return true;
}
void printNode(node* n) {
    dbg::printm(MODULE, "Node %p:\n", n);
    dbg::printm(MODULE, "  Size: %zu, Alloc: %zu, Freed: %zu\n", n->size, n->allocSize,
                n->freedSize);
    dbg::printm(MODULE, "  Status: %s\n", n->free ? "FREE" : "ALLOCATED");
    dbg::printm(MODULE, "  Range: [%p, %p]\n", n,
                reinterpret_cast<uint8_t*>(n) + sizeof(node) + n->size);
#if PRINT_PADDING
    dbg::printf("Pre padding:\n    ");
    for (uint64_t i = 0; i < sizeof(n->prePadding); ++i) {
        dbg::printf("%02.2hhx ", ((uint8_t*)n->prePadding)[i]);
        if ((i + 1) % 8 == 0) {
            dbg::printf("\n    ");
        }
    }
    dbg::printf("Post padding:\n    ");
    for (uint64_t i = 0; i < sizeof(n->postPadding); ++i) {
        dbg::printf("%02.2hhx ", ((uint8_t*)n->postPadding)[i]);
        if ((i + 1) % 8 == 0) {
            dbg::printf("\n    ");
        }
    }
#endif
}
static void sanityCheck() {
    node* current = __head;
    while (current) {
        if (!validateNode(current)) {
            dbg::printm(MODULE, "Invalid node at %p\n", current);
            printNode(current);
            std::abort();
        }
        if (current->next && current->next->prev != current) {
            dbg::printm(MODULE, "Linked list corruption detected!\n");
            std::abort();
        }
        current = current->next;
    }
}
bool isInitialized() {
    return __initialized;
}

void* allocate(size_t size, size_t align) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize(PMM_SIZE, VMM_MAX);
    }
    sanityCheck();
    if (size == 0) {
        dbg::printm(MODULE, "Allocation size cannot be zero\n");
        return nullptr;
    }
    if (align < MINIMUM_ALIGN_SIZE && (align & MINIMUM_ALIGN_SIZE) != 0) {
        align = ALIGNUP(align, MINIMUM_ALIGN_SIZE);
    }
    size_t alignedSize = ALIGNUP(size, align);
    node*  current     = __head;
    while (current) {
        if (!validateNode(current)) {
            dbg::printm(MODULE, "Corrupted block detected at %p\n", current);
            printNode(current);
            std::abort();
        }
        if (current->free && current->size >= alignedSize) {
            if (current->size > alignedSize + sizeof(node)) {
                node* newNode =
                    reinterpret_cast<node*>((uint64_t)current + sizeof(node) + alignedSize);
                newNode->freedSize = 0;
                newNode->allocSize = current->size - alignedSize - sizeof(node);
                newNode->size      = current->size - alignedSize - sizeof(node);
                newNode->free      = true;
                memset(newNode->prePadding, PADDING_PATTERN & 0xFF, sizeof(newNode->prePadding));
                memset(newNode->postPadding, PADDING_PATTERN & 0xFF, sizeof(newNode->postPadding));

                newNode->prev = current;
                newNode->next = current->next;
                current->next = newNode;
                if (newNode->next && newNode->next->prev) {
                    newNode->next->prev = newNode;
                }
            }
            current->allocSize = alignedSize;
            current->size      = alignedSize;
            current->free      = false;
            current->freedSize = 0;
            sanityCheck();
            dbg::popTrace();
            return reinterpret_cast<void*>((uint64_t)current + sizeof(node));
        }
        current = current->next;
    }
    dbg::printm(MODULE, "TODO: Extend the heap to fit a block of size %llu\n", alignedSize);
    std::abort();
}
void free(void* ptr, size_t size, size_t align) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        dbg::printm(MODULE, "Free called before allocator initialized\n");
        std::abort();
    }
    sanityCheck();
    if (!ptr) {
        dbg::printm(MODULE, "Free called with nullptr\n");
        std::abort();
    }
    node* block = reinterpret_cast<node*>((uintptr_t)ptr - sizeof(node));
    if (!validateNode(block)) {
        dbg::printm(MODULE, "Corrupted block detected at %p\n", block);
        printNode(block);
        std::abort();
    }
    if (block->free) {
        dbg::printm(MODULE, "Double free detected\n");
        std::abort();
    }
    if (block->allocSize != block->size) {
        dbg::printm(MODULE, "Corrupted block detected (block->allocSize != block->size)\n");
        std::abort();
    }
    size_t alignedSize = ALIGNUP(size, align);
    if (alignedSize > block->allocSize - block->freedSize) {
        dbg::printm(MODULE, "Invalid free size (requested = %zu, allocated = %zu)\n", alignedSize,
                    block->allocSize);
        std::abort();
    }
    block->freedSize += alignedSize;
    if (block->freedSize == block->allocSize) {
        block->free = true;
    }
    sanityCheck();
    dbg::popTrace();
}
static size_t getNextPowerOfTwo(size_t size) {
    if (size <= 1) return 1;
    size_t power = 1;
    while (power < size) {
        power <<= 1;
    }
    return power;
}
static size_t getAlignSize(size_t size) {
    size_t alignedSize = getNextPowerOfTwo(size);
    if (alignedSize < MINIMUM_ALIGN_SIZE) {
        alignedSize = MINIMUM_ALIGN_SIZE;
    }
    return alignedSize;
}
void* allocate(size_t size) {
    return allocate(size, getAlignSize(size));
}
void free(void* ptr, size_t size) {
    free(ptr, size, getAlignSize(size));
}
void free(void* ptr) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        dbg::printm(MODULE, "Called free before initialized\n");
        return;
    }
    node* block = reinterpret_cast<node*>((uintptr_t)ptr - sizeof(node));
    free(ptr, block->allocSize);
    dbg::popTrace();
}
void printInfo() {
    dbg::printm(MODULE, "INFO\n");
    uint64_t freeMemory = 0, allocatedMemory = 0, blocks = 0;
    node*    current = __head;
    while (current) {
        if (current->free) {
            freeMemory += current->freedSize == 0 ? current->size : current->freedSize;
        } else {
            allocatedMemory += current->allocSize;
        }
        blocks++;
        current = current->next;
    }
    dbg::printm(MODULE, "Currently free memory: %lu\n", freeMemory);
    dbg::printm(MODULE, "Currently allocated memory: %lu\n", allocatedMemory);
    dbg::printm(MODULE, "Current number of blocks: %lu\n", blocks);
}
}; // namespace mmu::heap