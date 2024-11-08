#include <kernel/driver/driver.h>
#include <common/dbg/dbg.h>
#include <stl/vector>
#include <cstdlib>
#define MODULE "Driver Manager"

namespace driver{
    deviceType Driver::getDeviceType(){
        return this->__device_type;
    }
    static bool initialized = false;
    static std::vector<Driver*> drivers;
    static Driver* loadDriver(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        switch(device->classCode){
            case 0x1: {
                switch(device->subclassCode){
                    case 0x1: {} break;
                    default: {
                        dbg::printm(MODULE, "TODO: Load mass storage controller for subclass %x\n", device->subclassCode);
                        std::abort();
                    } break;
                }
            } break;
            case 0x6: {
                dbg::printm(MODULE, "Skipping bridges\n");
            } break;
            default: {
                dbg::printm(MODULE, "TODO: Load driver for class %x\n", device->classCode);
                std::abort();
            } break;
        }
        dbg::popTrace();
        return nullptr;
    }
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Initializing...\n");
        drivers.clear();
        std::vector<pci::device*> pciDevices = pci::getAllDevices();
        for(pci::device* device : pciDevices){
            Driver* driver = loadDriver(device);
            if(driver == nullptr){
                continue;
            }
            drivers.push_back(loadDriver(device));
        }
        dbg::printm(MODULE, "Initialized\n");
        dbg::popTrace();
    }
    bool isInitialized(){
        return initialized;
    }
    size_t getDevicesCount(deviceType type){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        size_t count = 0;
        for(Driver* driver : drivers){
            if(driver->getDeviceType() == type){
                count++;
            }
        }
        dbg::popTrace();
        return count;
    }
    std::vector<Driver*> getDrivers(deviceType type){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        std::vector<Driver*> retDrivers;
        for(Driver* driver : drivers){
            if(driver->getDeviceType() == type){
                retDrivers.push_back(driver);
            }
        }
        dbg::popTrace();
        return retDrivers;
    }
};