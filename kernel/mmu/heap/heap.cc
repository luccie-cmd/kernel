/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <common/dbg/dbg.h>
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

namespace mmu::heap
{
    static bool __initialized = false;
    static node *__head = nullptr;
    static uint64_t __pmmSize, __vmmMax;
    void initialize(uint64_t pmm_size, uint64_t vmm_max)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        __pmmSize = pmm_size;
        __vmmMax = vmm_max;
        uint64_t base = pmm::allocVirtual(__pmmSize);
        __head = (node *)base;
        __head->free = true;
        __head->freedSize = 0;
        __head->allocSize = pmm_size;
        __head->size = pmm_size;
        __head->next = nullptr;
        __head->prev = nullptr;
        __initialized = true;
        dbg::popTrace();
    }
    bool isInitialized()
    {
        return __initialized;
    }
    size_t getNextPowerOfTwo(size_t size)
    {
        if (size <= 1)
            return 1;
        size_t power = 1;
        while (power < size)
        {
            power <<= 1;
        }
        return power;
    }
    size_t getAlignSize(size_t size)
    {
        const size_t CACHE_LINE_SIZE = 64;
        size_t alignedSize = getNextPowerOfTwo(size);
        if (alignedSize < CACHE_LINE_SIZE)
        {
            alignedSize = CACHE_LINE_SIZE;
        }
        return alignedSize;
    }
    void *allocate(size_t size)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        if (!isInitialized())
        {
            initialize(PMM_SIZE, VMM_MAX);
        }
        size_t ALIGN_SIZE = getAlignSize(size);
        size_t alignedLength = (size + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1);
        node *current = __head;
        while (current && current->next)
        {
            if (current->free && current->next->free)
            {
                current->size += sizeof(node) + current->next->size;
                current->next = current->next->next;
                if (current->next)
                {
                    current->next->prev = current;
                }
            }
            else
            {
                current = current->next;
            }
        }
        current = __head;
        while (current)
        {
            if (current->free && current->size == alignedLength)
            {
                if (current->size > alignedLength + sizeof(node))
                {
                    node *newNode = reinterpret_cast<node *>(reinterpret_cast<uint8_t *>(current) + sizeof(node) + alignedLength);
                    newNode->size = current->size - alignedLength - sizeof(node);
                    newNode->free = true;
                    newNode->next = current->next;
                    newNode->prev = current;
                    newNode->freedSize = 0;
                    newNode->allocSize = current->size - alignedLength - sizeof(node);
                    if (current->next)
                    {
                        current->next->prev = newNode;
                    }
                    current->next = newNode;
                    current->size = alignedLength;
                }
                current->freedSize = 0;
                current->allocSize = alignedLength;
                current->free = false;
                void *addr = reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(current) + sizeof(node));
                dbg::popTrace();
                return addr;
            }
            current = current->next;
        }
        current = __head;
        while (current)
        {
            if (current->free && current->size >= alignedLength)
            {
                if (current->size > alignedLength + sizeof(node))
                {
                    node *newNode = reinterpret_cast<node *>(reinterpret_cast<uint8_t *>(current) + sizeof(node) + alignedLength);
                    newNode->size = current->size - alignedLength - sizeof(node);
                    newNode->free = true;
                    newNode->next = current->next;
                    newNode->prev = current;
                    newNode->freedSize = 0;
                    newNode->allocSize = current->size - alignedLength - sizeof(node);
                    if (current->next)
                    {
                        current->next->prev = newNode;
                    }
                    current->next = newNode;
                    current->size = alignedLength;
                }
                current->freedSize = 0;
                current->allocSize = alignedLength;
                current->free = false;
                void *addr = reinterpret_cast<void *>(reinterpret_cast<uint8_t *>(current) + sizeof(node));
                dbg::popTrace();
                return addr;
            }
            current = current->next;
        }
        dbg::printm(MODULE, "Could not find suitable block with size %ld\n", alignedLength);
        dbg::printm(MODULE, "TODO: Extending of heap\n");
        std::abort();
    }
    void free(void *ptr, size_t size)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        if (!isInitialized())
        {
            dbg::printm(MODULE, "Called free before initialized\n");
            std::abort();
        }
        bool found = false;
        node *current = __head;
        while (current)
        {
            if (current->freedSize == current->allocSize)
            {
                current->freedSize = current->allocSize;
                current->free = true;
            }
            if (current == (node *)((uint64_t)ptr - sizeof(node)))
            {
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            dbg::printm(MODULE, "Tried freeing a node that was allocated elsewhere (ptr: 0x%llx ptr2: 0x%llx size: 0x%llx)\n", ptr, (uint64_t)ptr - sizeof(node), size);
            std::abort();
        }
        size_t ALIGN_SIZE = getAlignSize(size);
        size_t alignedLength = (size + ALIGN_SIZE - 1) & ~(ALIGN_SIZE - 1);
        node *freeNode = reinterpret_cast<node *>((uintptr_t)ptr - sizeof(node));
        if (freeNode->free)
        {
            dbg::printm(MODULE, "Called free on an already free node\n");
            std::abort();
        }
        if (freeNode->allocSize < alignedLength + freeNode->freedSize)
        {
            dbg::printm(MODULE, "Called free with larger then supposed to size\n");
            dbg::printm(MODULE, "Freenode allocSize: 0x%llx freedSize: 0x%llx size: 0x%llx calledSize: 0x%llx\n", freeNode->allocSize, freeNode->freedSize, freeNode->size, size);
            std::abort();
        }
        if (alignedLength == freeNode->size || alignedLength == 0)
        {
            freeNode->freedSize = freeNode->allocSize;
        }
        else
        {
            freeNode->freedSize += alignedLength;
        }
        current = __head;
        while (current)
        {
            if (current->freedSize == current->allocSize)
            {
                current->freedSize = current->allocSize;
                current->free = true;
            }
            current = current->next;
        }
        current = __head;
        while (current && current->next)
        {
            if (current->free && current->next->free)
            {
                current->size += sizeof(node) + current->next->size;
                current->next = current->next->next;
                if (current->next)
                {
                    current->next->prev = current;
                }
            }
            else
            {
                current = current->next;
            }
        }
        dbg::popTrace();
    }
    void free(void *ptr)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        if (!isInitialized())
        {
            dbg::printm(MODULE, "Called free before initialized\n");
            std::abort();
        }
        free(ptr, 0);
        dbg::popTrace();
    }
    void printInfo()
    {
        dbg::printm(MODULE, "INFO\n");
        uint64_t freeMemory = 0, allocatedMemory = 0, blocks = 0;
        node *current = __head;
        while (current)
        {
            if (current->free)
            {
                freeMemory += current->size;
            }
            else
            {
                allocatedMemory += current->size;
            }
            blocks++;
            current = current->next;
        }
        dbg::printm(MODULE, "Currently free memory: %lu\n", freeMemory);
        dbg::printm(MODULE, "Currently allocated memory: %lu\n", allocatedMemory);
        dbg::printm(MODULE, "Current number of blocks: %lu\n", blocks);
    }
};