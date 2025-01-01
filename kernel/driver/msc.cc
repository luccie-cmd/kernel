#include <kernel/driver/msc.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <drivers/ide.h>
#define MODULE "MSC Driver manager"

namespace driver{
    uint64_t getNewIdentIndex(){
        return 0;
    }
    MSCDriver::MSCDriver(StorageType type) :Driver(driverType::BLOCK), __identifier_index(getNewIdentIndex()) {}
    MSCDriver::~MSCDriver() {}
    MSCDriver* loadMSCdriver(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        MSCDriver* mscDriver = nullptr;
        if(device->classCode != 1){
            dbg::printm(MODULE, "Not a mass storage controller device\n");
        }
        switch(device->subclassCode){
            case 0x1: {
                mscDriver = drivers::loadIDEController(device);
            } break;
            case 0x6: {
                // mscDriver = drivers::loadSATAcontroller(device);
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