#include <kernel/driver/driver.h>
#include <common/dbg/dbg.h>
#define MODULE "Driver Manager"

namespace driver{
    static bool initialized = false;
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
        return 0;
    }
};