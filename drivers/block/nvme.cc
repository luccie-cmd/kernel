#include <drivers/block/nvme.h>
#include <cstdlib>
#include <common/dbg/dbg.h>

namespace drivers::block{
    NVMeDriver* loadNVMeDriver(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        std::abort();
        dbg::popTrace();
    }
};