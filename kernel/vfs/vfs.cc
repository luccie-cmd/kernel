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
        dbg::printm(MODULE, "Initializing...\n");
        readGPT();
        dbg::printm(MODULE, "Initialized\n");
        dbg::popTrace();
    }
    void readGPT(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(driver::getDevicesCount(driver::driverType::BLOCK) == 0){
            dbg::printm(MODULE, "No disks avaliable to read GPT\n");
            std::abort();
        }
        driver::Driver* blockDriver = driver::getDrivers(driver::driverType::BLOCK).at(0);
        dbg::printm(MODULE, "TODO: Read GPT\n");
        std::abort();
        dbg::popTrace();
    }
    void mount(uint8_t disk, uint8_t partition, const char* mountLocation){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(disk+1 > (int)driver::getDevicesCount(driver::driverType::BLOCK)){
            dbg::printm(MODULE, "Cannot access disk %d, out of range", disk+1);
            std::abort();
        }
        dbg::printm(MODULE, "TODO: Mount filesystem\n");
        dbg::popTrace();
    }
    std::FILE* openFile(const char* path){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "TODO: Open file\n");
        std::abort();
        dbg::popTrace();
    }
};