#include <kernel/driver/driver.h>
#include <common/dbg/dbg.h>
#include <stl/vector>
#include <cstdlib>
#define MODULE "Driver Manager"

namespace driver{
    static bool initialized = false;
    static std::vector<Driver*> drivers;
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm("Initializing...\n", MODULE);
        drivers.clear();
        dbg::printm("TODO: Scan PCI bus to load device drivers\n", MODULE);
        std::abort();
        dbg::printm("Initialized\n", MODULE);
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
            if(driver->getType() == type){
                count++;
            }
        }
        dbg::popTrace();
        return count;
    }
};