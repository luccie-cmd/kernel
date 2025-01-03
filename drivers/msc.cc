#include <drivers/msc.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <drivers/block/ide.h>
#include <drivers/block/nvme.h>
#define MODULE "MSC Driver manager"

namespace drivers{
    uint64_t getNewIdentIndex(){
        return 0;
    }
    MSCDriver::MSCDriver(StorageType type) :Driver(driver::driverType::BLOCK), __identifier_index(getNewIdentIndex()) {}
    MSCDriver::~MSCDriver() {}
    MSCDriver* loadMSCdriver(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        MSCDriver* mscDriver = nullptr;
        if(device->classCode != 1){
            dbg::printm(MODULE, "Not a mass storage controller device\n");
        }
        switch(device->subclassCode){
            case 0x1: {
                mscDriver = drivers::block::loadIDEController(device);
            } break;
            case 0x6: {
                dbg::printm(MODULE, "No, no SATA. F off\n");
            } break;
            case 0x8: {
                mscDriver = drivers::block::loadNVMeDriver(device);
            } break;
            default: {
                dbg::printm(MODULE, "TODO: Load MSC subclass %x\n", device->subclassCode);
                std::abort();
            } break;
        }
        dbg::popTrace();
        return mscDriver;
    }
};