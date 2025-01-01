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
#include <vector>
#include <utility>
#include <kernel/task/task.h>
#define MODULE "MMU HEAP"
#define PMM_SIZE MEGABYTE
#define VMM_MAX  GIGABYTE
#define ALGIN_SIZE     32

namespace mmu::heap{
    static bool __initialized = false;
    static node* __head = nullptr;
    static uint64_t __pmmSize, __vmmMax;
    void initialize(uint64_t pmm_size, uint64_t vmm_max){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Initializing...\n");
        __pmmSize = pmm_size;
        __vmmMax = vmm_max;
        uint64_t base = pmm::allocate(); 
        for(uint64_t pageOffset = 0; pageOffset < __pmmSize; pageOffset += PAGE_SIZE){
            uint64_t page = pmm::allocate();
            vmm::mapPage(vmm::getPML4(KERNEL_PID), page, base+pageOffset, PROTECTION_RW | PROTECTION_NOEXEC | PROTECTION_KERNEL, MAP_GLOBAL | MAP_PRESENT);
        }
        __head = (node*)base;
        __head->free = true;
        __head->freedSize = 0;
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
                if(current->size > alignedLength + sizeof(node)){
                    node* newNode = reinterpret_cast<node*>(reinterpret_cast<uint8_t*>(current)+sizeof(node)+alignedLength);
                    newNode->size = current->size - alignedLength - sizeof(node);
                    newNode->free = true;
                    newNode->freedSize = 0;
                    newNode->next = current->next;
                    newNode->prev = current;
                    if (current->next){
                        current->next->prev = newNode;
                    }
                    current->next = newNode;
                    current->size = alignedLength;
                }
                current->free = false;
                void* addr = reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(current)+sizeof(node));
                if(task::getCurrentPID() != KERNEL_PID){
                    vmm::mapPage(vmm::getPML4(task::getCurrentPID()), vmm::getPhysicalAddr(vmm::getPML4(task::getCurrentPID()), (uint64_t)current), (size_t)current, PROTECTION_RW | (task::getCurrentPID() == KERNEL_PID ? PROTECTION_KERNEL : 0), MAP_GLOBAL | MAP_PRESENT);
                }
                dbg::popTrace();
                return addr;
            }
            current = current->next;
        }
        dbg::printm(MODULE, "Could not find suitable block with size %ld\n", alignedLength);
        dbg::printm(MODULE, "TODO: Extending of heap\n");
        current = __head;
        while(current){
            dbg::printm(MODULE, "Addr: %llx Size: %llu %s\n", current, current->size, current->free ? "free" : "in use");
            current = current->next;
        }
        std::abort();
    }
    void free(void* ptr, size_t size){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize(PMM_SIZE, VMM_MAX);
        }
        bool found = false;
        node* current = __head;
        node* freeNode = reinterpret_cast<node*>((uintptr_t)ptr-sizeof(node));
        while(current){
            found = current == freeNode;
            if(found){
                break;
            }
            current = current->next;
        }
        if (!found) {
            dbg::printm(MODULE, "Invalid pointer passed to free: 0x%llx, attempted node to free: %llx\n", ptr, freeNode);
            current = __head;
            while(current){
                dbg::printm(MODULE, "Addr: %llx Size: %llu %s\n", current, current->size, current->free ? "free" : "in use");
                current = current->next;
            }
            std::abort();
        }
        if(size == freeNode->size || size == 0){
            freeNode->freedSize = freeNode->size;
        } else{
            freeNode->freedSize += size;
        }
        if(freeNode->freedSize == freeNode->size){
            freeNode->free = true;
        }
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