/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/mmu/mmu.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <cstdint>
#define MODULE "MMU HEAP"
#define PMM_SIZE MEGABYTE
#define VMM_MAX  GIGABYTE

namespace mmu::heap{
    static bool initialized = false;
    static node* head = nullptr;
    static uint64_t pmmSize, vmmMax;
    void initialize(uint64_t pmm_size, uint64_t vmm_max){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm("Initializing...\n", MODULE);
        pmmSize = pmm_size;
        vmmMax = vmm_max;
        vmm::switchPML4(KERNEL_PID);
        uint64_t base = pmm::allocate();
        for(uint64_t pageOffset = 0; pageOffset < pmmSize; pageOffset += PAGE_SIZE){
            uint64_t page = pmm::allocate();
            vmm::mapPage(page, base+page, PROTECTION_RW | PROTECTION_NOEXEC | PROTECTION_KERNEL, MAP_GLOBAL | MAP_PRESENT);
        }
        head = (node*)base;
        head->free = true;
        head->size = pmm_size;
        head->next = nullptr;
        head->prev = nullptr;
        initialized = true;
        dbg::printm("Initialized\n", MODULE);
        dbg::popTrace();
    }
    bool isInitialized(){
        return initialized;
    }
    void* allocate(size_t size){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize(PMM_SIZE, VMM_MAX);
        }
        dbg::printm("TODO: Allocate memory\n", MODULE);
        std::abort();
        dbg::popTrace();
    }
    void free(void* ptr, size_t size){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize(PMM_SIZE, VMM_MAX);
        }
        dbg::printm("TODO: Free memory\n", MODULE);
        std::abort();
        dbg::popTrace();}
    void free(void* ptr){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize(PMM_SIZE, VMM_MAX);
        }
        free(ptr, 0);
        dbg::popTrace();
    }
};