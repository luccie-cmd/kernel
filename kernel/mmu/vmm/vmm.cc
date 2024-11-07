/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/mmu/mmu.h>
#include <kernel/mmu/vmm/types.h>
#include <common/dbg/dbg.h>
#include <../limine/limine.h>
#include <cstdlib>
#include <common/io/io.h>
#define MODULE "MMU VMM"

namespace mmu::vmm{
    static uint64_t HHDMoffset;
    static bool initialized;
    limine_hhdm_request hhdm_request = {
        .id = LIMINE_HHDM_REQUEST,
        .revision = 0,
        .response = nullptr,
    };
    static uint64_t CR3LookupTable[MAX_PIDS];
    static PML4* currentPML4;
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm("Initializing...\n", MODULE);
        if(hhdm_request.response == nullptr){
            dbg::printm("Bootloader failed to set HHDM response\n", MODULE);
            std::abort();
        }
        HHDMoffset = hhdm_request.response->offset;
        initialized = true;
        dbg::printm("Initialized\n", MODULE);
        dbg::popTrace();
    }
    bool isInitialized(){
        return initialized;
    }
    void switchPML4(task::pid_t pid){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        if(pid > MAX_PIDS){
            dbg::printm("Invalid PID\n", MODULE);
            std::abort();
        }
        if(pid == KERNEL_PID && CR3LookupTable[pid] == 0){
            CR3LookupTable[pid] = io::rcr3();
        }
        if(CR3LookupTable[pid] == 0){
            uint64_t cr3 = pmm::allocate();
            CR3LookupTable[pid] = cr3;
        }
        uint64_t cr3 = CR3LookupTable[pid];
        cr3 &= ~(0xfff);
        cr3 += HHDMoffset;
        currentPML4 = reinterpret_cast<PML4*>(cr3);
        io::wcr3(CR3LookupTable[pid]);
        dbg::popTrace();
    }
    uint64_t makeVirtual(uint64_t addr){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        dbg::popTrace();
        return addr+HHDMoffset;
    }
    uint64_t getHHDM(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        dbg::popTrace();
        return HHDMoffset;
    }
    void mapPage(size_t physicalAddr, size_t virtualAddr, int protFlags, int mapFlags){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        dbg::printm("TODO: Mapping of pages\n", MODULE);
        std::abort();
        dbg::popTrace();
    }
};