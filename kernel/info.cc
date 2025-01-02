#include <kernel/info.h>
#include <kernel/hal/pci/pci.h>
#include <kernel/mmu/mmu.h>
#include <kernel/vfs/vfs.h>

namespace info{
    void printInfo(){
        pci::printInfo();
        mmu::heap::printInfo();
        mmu::pmm::printInfo();
        vfs::printInfo();
    }
};