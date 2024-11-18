#include <kernel/vfs/vfs.h>
#include <kernel/vfs/gpt.h>
#include <common/dbg/dbg.h>
#include <kernel/driver/driver.h>
#include <kernel/driver/msc.h>
#include <cstdlib>
#include <cstring>
#define MODULE "VFS"

namespace vfs{
    bool inited = false;
    std::vector<std::vector<PartitionEntry*>> partitionEntries;
    bool isInitialized(){
        return inited;
    }
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Initializing...\n");
        for(std::vector<PartitionEntry*> entries : partitionEntries){
            entries.clear();
        }
        partitionEntries.clear();
        dbg::printm(MODULE, "Initialized\n");
        dbg::popTrace();
    }
    void readGPT(uint8_t disk){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(driver::getDevicesCount(driver::driverType::BLOCK) == 0){
            dbg::printm(MODULE, "No disks avaliable to read GPT\n");
            std::abort();
        }
        driver::MSCDriver* blockDriver = reinterpret_cast<driver::MSCDriver*>(driver::getDrivers(driver::driverType::BLOCK).at(0));
        uint8_t *legacyMBR = new uint8_t[512];
        PartitionTableHeader *PTH = new PartitionTableHeader;
        std::memset(legacyMBR, 0, 512);
        if(!blockDriver->read(0, 0, 1, legacyMBR)){
            dbg::printm(MODULE, "ERROR: Failed to read legacy MBR\n");
            std::abort();
        }
        delete[] legacyMBR;
        if(!blockDriver->read(0, 1, 1, PTH)){
            dbg::printm(MODULE, "ERROR: Failed to read partition table header\n");
            std::abort();
        }
        uint8_t* buffer = new uint8_t[15872];
        if(!blockDriver->read(0, 2, 31, buffer)){
            dbg::printm(MODULE, "ERROR: Failed to read partition entries\n");
            std::abort();
        }
        std::vector<PartitionEntry*> entries;
        for(uint32_t i = 0; i < 15872; i+=sizeof(PartitionEntry)){
            PartitionEntry* entry = (PartitionEntry*)(&buffer[i]);
            if(entry->startLBA == 0 && entry->endLBA == 0){
                break;
            }
            entries.push_back(entry);
        }
        partitionEntries.push_back(entries);
        dbg::popTrace();
    }
    void mount(uint8_t disk, uint8_t partition, const char* mountLocation){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(disk+1 > (int)driver::getDevicesCount(driver::driverType::BLOCK)){
            dbg::printm(MODULE, "Cannot access disk %d, out of range", disk+1);
            std::abort();
        }
        if(!isInitialized()){
            initialize();
        }
        if(partitionEntries.count() < disk && partitionEntries.at(disk).count() < partition){
            readGPT(disk);
        }
        if(partitionEntries.at(disk).count() < partition){
            dbg::printm(MODULE, "ERROR: Partition is out of the possible partitions\n");
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