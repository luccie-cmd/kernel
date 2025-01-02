/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/mmu/mmu.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <../limine/limine.h>
#define MODULE "MMU PMM"

namespace mmu::pmm{
    static bool __initialized = false;
    limine_memmap_request memmap_request = {
        .id = LIMINE_MEMMAP_REQUEST,
        .revision = 0,
        .response = nullptr
    };
    static uint64_t __totalUseableMemory = 0;
    static uint64_t __totalUsedMemory = 0;
    static node* __head = nullptr;
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Initializing...\n");
        if(memmap_request.response == nullptr){
            dbg::printm(MODULE, "Bootloader failed to set memory map response\n");
            std::abort();
        }
        uint64_t memmap_entries = memmap_request.response->entry_count;
        for(uint64_t i = 0; i < memmap_entries; ++i){
            limine_memmap_entry* entry = memmap_request.response->entries[i];
            if(entry->type == LIMINE_MEMMAP_USABLE && entry->length >= PAGE_SIZE){
                __totalUseableMemory += entry->length;
                node* newNode = (node*)vmm::makeVirtual(entry->base);
                newNode->size = entry->length;
                if(__head == nullptr){
                    __head = newNode;
                } else{
                    newNode->next = __head;
                    __head = newNode;
                }
            }
        }
        __totalUsedMemory = 0;
        dbg::printm(MODULE, "Initialized with 0x%llx pages\n", __totalUseableMemory/PAGE_SIZE);
        __initialized = true;
        dbg::popTrace();
    }
    bool isInitialized(){
        return __initialized;
    }
    uint64_t allocVirtual(uint64_t size){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        node* current = __head;
        node* prev = nullptr;
        while(current){
            if(current->size >= size){
                uint64_t addr = (uint64_t)current;
                if(current->size > size){
                    node* newNode = (node*)(addr+size);
                    newNode->size = current->size-size;
                    newNode->next = current->next;
                    if(prev != nullptr){
                        prev->next = newNode;
                    } else{
                        __head = newNode;
                    }
                } else{
                    if(prev != nullptr){
                        prev->next = current->next;
                    } else{
                        __head = current->next;
                    }
                }
                __totalUsedMemory += size;
                dbg::popTrace();
                return addr - vmm::getHHDM();
            }
            prev = current;
            current = current->next;
        }
        dbg::printm(MODULE, "Could not find a physical address for size 0x%llx\n", size);
        std::abort();
        dbg::popTrace();
    }
    uint64_t allocate(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint64_t addr = allocVirtual(PAGE_SIZE);
        dbg::popTrace();
        return addr;
    }
    void printInfo(){
        dbg::printm(MODULE, "INFORMATION\n");
        dbg::printm(MODULE, "Allocated pages: %llu\n", __totalUsedMemory/PAGE_SIZE);
        node* current = __head;
        while(current){
            dbg::printm(MODULE, "Size: %llu\n", current->size);
            current = current->next;
        }
    }
}