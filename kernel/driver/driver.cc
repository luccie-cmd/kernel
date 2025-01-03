#include <kernel/driver/driver.h>
#include <common/dbg/dbg.h>
#include <vector>
#include <cstdlib>
#include <drivers/msc.h>
#define MODULE "Driver manager"

namespace driver{
    driverType Driver::getDeviceType(){
        return this->__driver_type;
    }
    static bool initialized = false;
    static std::vector<Driver*> drivers;
    static Driver* loadDriver(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        Driver* driver = nullptr;
        switch(device->classCode){
            case 0x1: {
                driver = drivers::loadMSCdriver(device);
            } break;
            case 0x2: {
                dbg::printm(MODULE, "TODO: Setup networking\n");
            } break;
            case 0x3: {
                dbg::printm(MODULE, "TODO: display drivers\n");
            } break;
            case 0xc:
            case 0x6: {
                dbg::printm(MODULE, "Not loading bridge drivers\n");
            } break;
            default: {
                dbg::printm(MODULE, "TODO: Load driver for class %x\n", device->classCode);
                std::abort();
            } break;
        }
        dbg::popTrace();
        return driver;
    }
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        drivers.clear();
        std::vector<pci::device*> pciDevices = pci::getAllDevices();
        for(pci::device* device : pciDevices){
            Driver* driver = loadDriver(device);
            if(driver == nullptr){
                continue;
            }
            driver->init(device);
            drivers.push_back(driver);
        }
        initialized = true;
        dbg::popTrace();
    }
    bool isInitialized(){
        return initialized;
    }
    size_t getDevicesCount(driverType type){
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
    std::vector<Driver*> getDrivers(driverType type){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        if(getDevicesCount(type) == 0){
            dbg::printm(MODULE, "ERROR: Unable to search for device type %d if there are no devices like that\n", (int)type);
            std::abort();
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
    Driver::Driver(driverType type) :__driver_type(type) {}
    Driver::~Driver() {}
    void Driver::setDriverName(const char* name){
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->__driver_name = name;
        dbg::popTrace();
    }
};