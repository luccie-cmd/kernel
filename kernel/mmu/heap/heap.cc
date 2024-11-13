/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/mmu/mmu.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#define MODULE "MMU HEAP"
#define PMM_SIZE MEGABYTE
#define VMM_MAX  GIGABYTE
#define ALGIN_SIZE     16

namespace mmu::heap{
    static bool __initialized = false;
    static node* __head = nullptr;
    static uint64_t __pmmSize, __vmmMax;
    void initialize(uint64_t pmm_size, uint64_t vmm_max){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Initializing...\n");
        __pmmSize = pmm_size;
        __vmmMax = vmm_max;
        vmm::switchPML4(KERNEL_PID);
        uint64_t base = pmm::allocate();
        for(uint64_t pageOffset = 0; pageOffset < __pmmSize; pageOffset += PAGE_SIZE){
            uint64_t page = pmm::allocate();
            vmm::mapPage(page, base+page, PROTECTION_RW | PROTECTION_NOEXEC | PROTECTION_KERNEL, MAP_GLOBAL | MAP_PRESENT);
        }
        __head = (node*)base;
        __head->free = true;
        __head->size = pmm_size;
        __head->next = nullptr;
        __head->prev = nullptr;
        __initialized = true;
        dbg::printm(MODULE, "Initialized with 1MB initial memory and 1GB maximum memory\n");
        dbg::popTrace();
    }
    bool isInitialized(){
        return __initialized;
    }
    void* allocate(size_t size){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize(PMM_SIZE, VMM_MAX);
        }
        size_t alignedLength = (size + ALGIN_SIZE - 1) & ~(ALGIN_SIZE - 1);
        node* current = __head;
        while(current){
            if(current->free && current->size >= alignedLength){
                if(current->size > alignedLength){
                    node* newNode = reinterpret_cast<node*>(current+sizeof(node)+alignedLength);
                    newNode->size = current->size - alignedLength - sizeof(node);
                    newNode->free = true;
                    newNode->next = current->next;
                    newNode->prev = current;

                    if (current->next){
                        current->next->prev = newNode;
                    }
                    current->next = newNode;

                    current->size = alignedLength;
                }
                current->free = false;
                dbg::popTrace();
                return reinterpret_cast<void*>(current+sizeof(node));
            }
            current = current->next;
        }
        dbg::printm(MODULE, "Could not find suitable block with size %ld\n", alignedLength);
        dbg::printm(MODULE, "TODO: Extending of heap\n");
        std::abort();
        dbg::popTrace();
    }
    void free(void* ptr, size_t size){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize(PMM_SIZE, VMM_MAX);
        }
        node* freeNode = reinterpret_cast<node*>((uint64_t)ptr-sizeof(node));
        if(size == freeNode->size || size == 0){
            freeNode->free = true;
            dbg::popTrace();
            return;
        }
        dbg::printm(MODULE, "TODO: arbitrary freeing of memory sizes\n");
        std::abort();
        dbg::popTrace();
    }
    void free(void* ptr){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize(PMM_SIZE, VMM_MAX);
        }
        free(ptr, 0);
        dbg::popTrace();
    }
};