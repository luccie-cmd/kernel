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
#include <cstring>
#define MODULE "MMU VMM"

namespace mmu::vmm{
    static uint64_t __HHDMoffset;
    static bool __initialized;
    limine_hhdm_request hhdm_request = {
        .id = LIMINE_HHDM_REQUEST,
        .revision = 0,
        .response = nullptr,
    };
    static uint64_t __CR3LookupTable[MAX_PIDS];
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Initializing...\n");
        if(hhdm_request.response == nullptr){
            dbg::printm(MODULE, "Bootloader failed to set HHDM response\n");
            std::abort();
        }
        __HHDMoffset = hhdm_request.response->offset;
        __initialized = true;
        (void)getPML4(KERNEL_PID);
        dbg::printm(MODULE, "Initialized with level 4 paging\n");
        dbg::printm(MODULE, "Kernel PML4 address: 0x%llx\n", __CR3LookupTable[KERNEL_PID]);
        dbg::popTrace();
    }
    bool isInitialized(){
        return __initialized;
    }
    PML4* getPML4(task::pid_t pid){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        if(pid > MAX_PIDS){
            dbg::printm("Invalid PID\n", MODULE);
            std::abort();
        }
        if(pid == KERNEL_PID && __CR3LookupTable[pid] == 0){
            __CR3LookupTable[pid] = io::rcr3();
        }
        if(__CR3LookupTable[pid] == 0){
            uint64_t cr3 = pmm::allocate();
            __CR3LookupTable[pid] = cr3;
        }
        uint64_t cr3 = __CR3LookupTable[pid];
        cr3 &= ~(0xfff);
        cr3 += __HHDMoffset;
        dbg::popTrace();
        return reinterpret_cast<PML4*>(cr3);
    }
    uint64_t makeVirtual(uint64_t addr){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        dbg::popTrace();
        return addr+__HHDMoffset;
    }
    uint64_t getHHDM(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        dbg::popTrace();
        return __HHDMoffset;
    }
    static vmm_address getVMMfromVA(uint64_t vaddr){
        vmm_address result;
        result.padding = (vaddr >> 48) & 0xFFFF;
        result.pml4e = (vaddr >> 39) & 0x1FF;
        result.pdpe = (vaddr >> 30) & 0x1FF;
        result.pde = (vaddr >> 21) & 0x1FF;
        result.pte = (vaddr >> 12) & 0x1FF;
        result.offset = vaddr & 0xFFF;
        return result;
    }
    void mapPage(PML4* pml4, size_t physicalAddr, size_t virtualAddr, int prot, int map){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        vmm_address vma = getVMMfromVA(virtualAddr);
        bool kernelPage = (prot & PROTECTION_KERNEL) != 0;
        bool readWrite = (prot & PROTECTION_RW) != 0;
        bool execute = (prot & PROTECTION_NOEXEC) == 0;
        bool presentMap = (map & MAP_PRESENT) != 0;
        bool globalMap = (map & MAP_GLOBAL) != 0;
        if(!presentMap){
            dbg::printm(MODULE, "Cannot map a page that isn't present\n");
            std::abort();
        }
        if(!readWrite){
            dbg::printm(MODULE, "Cannot map a page that isn't readable nor writeable\n");
            std::abort();
        }
        if(pml4[vma.pml4e].pdpe_ptr == 0){
            uint64_t page = pmm::allocate();
            std::memset(reinterpret_cast<void*>(makeVirtual(page)), 0, PAGE_SIZE);
            pml4[vma.pml4e].pdpe_ptr = page >> 12;
        }
        PDPE* pdpe = reinterpret_cast<PDPE*>(makeVirtual(pml4[vma.pml4e].pdpe_ptr << 12));
        if(pdpe[vma.pdpe].pde_ptr == 0){
            uint64_t page = pmm::allocate();
            std::memset(reinterpret_cast<void*>(makeVirtual(page)), 0, PAGE_SIZE);
            pdpe[vma.pdpe].pde_ptr = page >> 12;
        }
        PDE* pde = reinterpret_cast<PDE*>(makeVirtual(pdpe[vma.pdpe].pde_ptr << 12));
        if(pde[vma.pde].pte_ptr == 0){
            uint64_t page = pmm::allocate();
            std::memset(reinterpret_cast<void*>(makeVirtual(page)), 0, PAGE_SIZE);
            pde[vma.pde].pte_ptr = page >> 12;
        }
        PTE* pte = reinterpret_cast<PTE*>(makeVirtual(pde[vma.pde].pte_ptr << 12));

        pml4[vma.pml4e].present = presentMap;
        pml4[vma.pml4e].no_execute = !execute;
        pml4[vma.pml4e].rw = readWrite;
        pml4[vma.pml4e].user = !kernelPage;
        pml4[vma.pml4e].pwt = 0;
        pml4[vma.pml4e].pcd = 0;
        pml4[vma.pml4e].accesed = 0;
        pml4[vma.pml4e].ignored = 0;
        pml4[vma.pml4e].mbz = 0;
        pml4[vma.pml4e].ats0 = 0;
        pml4[vma.pml4e].ats1 = 0;

        pdpe[vma.pdpe].present = presentMap;
        pdpe[vma.pdpe].no_execute = !execute;
        pdpe[vma.pdpe].rw = readWrite;
        pdpe[vma.pdpe].user = !kernelPage;
        pdpe[vma.pdpe].pwt = 0;
        pdpe[vma.pdpe].pcd = 0;
        pdpe[vma.pdpe].accesed = 0;
        pdpe[vma.pdpe].ignored = 0;
        pdpe[vma.pdpe].mbz = 0;
        pdpe[vma.pdpe].ats0 = 0;
        pdpe[vma.pdpe].ats1 = 0;

        pde[vma.pde].present = presentMap;
        pde[vma.pde].no_execute = !execute;
        pde[vma.pde].rw = readWrite;
        pde[vma.pde].user = !kernelPage;
        pde[vma.pde].pwt = 0;
        pde[vma.pde].pcd = 0;
        pde[vma.pde].accesed = 0;
        pde[vma.pde].ignored = 0;
        pde[vma.pde].mbz = 0;
        pde[vma.pde].ats0 = 0;
        pde[vma.pde].ats1 = 0;

        pte[vma.pte].present = presentMap;
        pte[vma.pte].no_execute = !execute;
        pte[vma.pte].rw = readWrite;
        pte[vma.pte].user = !kernelPage;
        pte[vma.pte].global = globalMap;
        pte[vma.pte].papn_ppn = physicalAddr >> 12;
        pte[vma.pte].pwt = 0;
        pte[vma.pte].pcd = 0;
        pte[vma.pte].accesed = 0;
        pte[vma.pte].dirty = 0;
        pte[vma.pte].pat = 0;
        pte[vma.pte].ats0 = 0;
        pte[vma.pte].ats1 = 0;
        pte[vma.pte].pkeys = 0;

        io::invalpg((void*)virtualAddr);
        dbg::popTrace();
    }
    uint64_t getPhysicalAddr(PML4* pml4, uint64_t addr){
        dbg::addTrace(__PRETTY_FUNCTION__);
        vmm_address vma = getVMMfromVA(addr);
        if(pml4[vma.pml4e].pdpe_ptr == 0){
            return 0;
        }
        PDPE* pdpe = reinterpret_cast<PDPE*>(makeVirtual(pml4[vma.pml4e].pdpe_ptr << 12));
        if(pdpe[vma.pdpe].pde_ptr == 0){
            return 0;
        }
        PDE* pde = reinterpret_cast<PDE*>(makeVirtual(pdpe[vma.pdpe].pde_ptr << 12));
        if(pde[vma.pde].pte_ptr == 0){
            return 0;
        }
        PTE* pte = reinterpret_cast<PTE*>(makeVirtual(pde[vma.pte].pte_ptr << 12));
        uint64_t retAddr = pte[vma.pte].papn_ppn << 12;
        dbg::popTrace();
        return retAddr; 
    }
};