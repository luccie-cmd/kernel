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
    static bool initialized = false;
    limine_memmap_request memmap_request = {
        .id = LIMINE_MEMMAP_REQUEST,
        .revision = 0,
        .response = nullptr
    };
    static node* head = nullptr;
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm("Initializing...\n", MODULE);
        if(memmap_request.response == nullptr){
            dbg::printm("Bootloader failed to set memory map response\n", MODULE);
            std::abort();
        }
        uint64_t memmap_entries = memmap_request.response->entry_count;
        for(uint64_t i = 0; i < memmap_entries; ++i){
            limine_memmap_entry* entry = memmap_request.response->entries[i];
            if(entry->type == LIMINE_MEMMAP_USABLE && entry->length >= PAGE_SIZE){
                node* newNode = (node*)vmm::makeVirtual(entry->base);
                newNode->size = entry->length;
                if(head == nullptr){
                    head = newNode;
                } else{
                    newNode->next = head;
                    head = newNode;
                }
            }
        }
        dbg::printm("Initialized\n", MODULE);
        initialized = true;
        dbg::popTrace();
    }
    bool isInitialized(){
        return initialized;
    }
    uint64_t allocate(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        node* current = head;
        node* prev = nullptr;
        while(current){
            if(current->size >= PAGE_SIZE){
                uint64_t addr = (uint64_t)current;
                if(current->size > PAGE_SIZE){
                    node* newNode = (node*)(current+PAGE_SIZE);
                    newNode->size = current->size-PAGE_SIZE;
                    newNode->next = current->next;
                    if(prev != nullptr){
                        prev->next = newNode;
                    } else{
                        head = newNode;
                    }
                } else{
                    if(prev != nullptr){
                        prev->next = current->next;
                    } else{
                        head = current->next;
                    }
                }
                dbg::popTrace();
                return addr - vmm::getHHDM();
            }
            prev = current;
            current = current->next;
        }
        dbg::printm("Could not find a physical address\n", MODULE);
        std::abort();
    }
}