#include <common/dbg/dbg.h>
#include <cstdlib>
#include <drivers/block/ide.h>
#include <drivers/block/nvme.h>
#include <drivers/msc.h>
#define MODULE "MSC Driver manager"

namespace drivers {
uint64_t drives = 0;
uint64_t getNewIdentIndex() {
    return ++drives;
}
MSCDriver::MSCDriver(StorageType type)
    : Driver(driver::driverType::BLOCK), __identifier_index(getNewIdentIndex()) {
    this->__storage_type = type;
}
MSCDriver::~MSCDriver() {}
MSCDriver* loadMSCdriver(pci::device* device) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    MSCDriver* mscDriver = nullptr;
    if (device->classCode != 1) {
        dbg::printm(MODULE, "Not a mass storage controller device\n");
    }
    switch (device->subclassCode) {
    case 0x1: {
        mscDriver = block::loadIDEController(device);
    } break;
    case 0x6: {
    } break;
    case 0x8: {
        mscDriver = block::loadNVMeDriver(device);
    } break;
    default: {
        dbg::printm(MODULE, "TODO: Load MSC subclass %x\n", device->subclassCode);
        std::abort();
    } break;
    }
    dbg::popTrace();
    return mscDriver;
}
}; // namespace drivers