#include <kernel/vfs/vfs.h>
#include <kernel/vfs/gpt.h>
#include <common/dbg/dbg.h>
#include <kernel/driver/driver.h>
#include <cstdlib>
#define MODULE "VFS"

namespace vfs{
    bool inited = false;
    bool isInitialized(){
        return inited;
    }
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm("Initializing...\n", MODULE);
        readGPT();
        dbg::printm("Initialized\n", MODULE);
        dbg::popTrace();
    }
    void readGPT(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(driver::getDevicesCount(driver::deviceType::BLOCK) == 0){
            dbg::printm("ERROR: No disks avaliable to read GPT\n", MODULE);
            std::abort();
        }
        // Driver* blockDrivers = driver::getDevices(driver::deviceType::BLOCK).at(0);
        dbg::popTrace();
    }
    void mount(uint8_t disk, uint8_t partition, const char* mountLocation){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(disk+1 > (int)driver::getDevicesCount(driver::deviceType::BLOCK)){
            dbg::printm("ERROR: Cannot access disk %d, out of range", MODULE);
            std::abort();
        }
        dbg::popTrace();
    }
    std::FILE* openFile(const char* path){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm("TODO: Open file\n", MODULE);
        std::abort();
        dbg::popTrace();
    }
};