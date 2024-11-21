#include <kernel/vfs/vfs.h>
#include <kernel/vfs/gpt.h>
#include <common/dbg/dbg.h>
#include <kernel/driver/driver.h>
#include <kernel/driver/msc.h>
#include <cstdlib>
#include <cstring>
#include <kernel/task/task.h>
#define MODULE "VFS"

namespace vfs{
    bool inited = false;
    std::vector<std::vector<PartitionEntry*>> partitionEntries;
    std::vector<MountPoint*> mountPoints;
    bool isInitialized(){
        return inited;
    }
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Initializing...\n");
        partitionEntries.clear();
        for(uint64_t i = 0; i < driver::getDevicesCount(driver::driverType::BLOCK); ++i){
            readGPT((uint8_t)i);
        }
        inited = true;
        dbg::printm(MODULE, "Initialized\n");
        dbg::popTrace();
    }
    uint8_t* parseGUID(uint8_t* GUID){
        uint8_t* newGUID = new uint8_t[16];
        newGUID[0] = GUID[3];
        newGUID[1] = GUID[2];
        newGUID[2] = GUID[1];
        newGUID[3] = GUID[0];
        newGUID[4] = GUID[5];
        newGUID[5] = GUID[4];
        newGUID[6] = GUID[7];
        newGUID[7] = GUID[6];
        newGUID[8] = GUID[8];
        newGUID[9] = GUID[9];
        newGUID[10] = GUID[10];
        newGUID[12] = GUID[12];
        newGUID[13] = GUID[13];
        newGUID[14] = GUID[14];
        newGUID[15] = GUID[15];
        return newGUID;
    }
    void readGPT(uint8_t disk){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(driver::getDevicesCount(driver::driverType::BLOCK) == 0){
            dbg::printm(MODULE, "No disks avaliable to read GPT\n");
            std::abort();
        }
        driver::MSCDriver* blockDriver = reinterpret_cast<driver::MSCDriver*>(driver::getDrivers(driver::driverType::BLOCK).at(disk));
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
        for(uint32_t i = 0; i < PTH->partitionCount; i++){
            PartitionEntry* entry = (PartitionEntry*)(buffer+(i*sizeof(PartitionEntry)));
            if(entry->startLBA == 0 && entry->endLBA == 0){
                break;
            }
            uint8_t* newGUID = parseGUID(entry->GUID);
            std::memcpy(entry->GUID, newGUID, sizeof(entry->GUID));
            dbg::printm(MODULE, "Entry %d = %llx\n", i, entry);
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
        if(partitionEntries.at(disk).count() < partition){
            dbg::printm(MODULE, "ERROR: Partition is out of the possible partitions\n");
            dbg::printm(MODULE, "Attempted to load partition %hhd but only %lld partitions were found\n", partition, partitionEntries.at(disk).count());
            std::abort();
        }
        drivers::FSDriver* fileSystemdriver = drivers::loadFSDriver(partitionEntries.at(disk).at(partition), reinterpret_cast<driver::MSCDriver*>(driver::getDrivers(driver::driverType::BLOCK).at(disk)));
        dbg::popTrace();
    }
    int openFile(const char* path, int flags){
        dbg::addTrace(__PRETTY_FUNCTION__);
        int handle = 0;
        for(MountPoint* mp : mountPoints){
            if(std::memcmp(path, mp->mountPath, strlen(mp->mountPath)) == 0){
                handle = mp->fileSystemDriver->open(task::getCurrentPID(), path, flags);
                break;
            }
        }
        if(handle == 0){
            dbg::printm(MODULE, "Unable to find file %s\n", path);
            std::abort();
        }
        dbg::popTrace();
        return handle;
    }
    void closeFile(int handle){
        dbg::addTrace(__PRETTY_FUNCTION__);
        
        dbg::popTrace();
    }
};