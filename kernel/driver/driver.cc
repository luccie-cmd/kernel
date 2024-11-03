#include <kernel/driver/driver.h>
#include <common/dbg/dbg.h>
#include <vector>
#define MODULE "Driver Manager"

namespace driver{
    static bool initialized = false;
    static std::vector<Driver*> drivers;
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm("Initializing...\n", MODULE);
        dbg::printm("Initialized\n", MODULE);
        dbg::popTrace();
    }
    bool isInitialized(){
        return initialized;
    }
    size_t getDevicesCount(deviceType type){
        size_t count = 0;
        for(Driver* driver : drivers){
            if(driver->getType() == type){
                count++;
            }
        }
        return count;
    }
};