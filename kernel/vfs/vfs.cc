#include <kernel/vfs/vfs.h>
#include <kernel/vfs/gpt.h>
#include <common/dbg/dbg.h>
#include <kernel/driver/driver.h>
#include <kernel/driver/msc.h>
#include <cstdlib>
#include <cstring>
#include <kernel/task/task.h>
#include <utility>
#include <cassert>
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
        mountPoints.clear();
        readGPT(0);
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
    static std::pair<driver::MSCDriver*, uint8_t> translateVirtualDiskToPhysicalDisk(uint8_t disk){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(driver::getDevicesCount(driver::driverType::BLOCK) == 0){
            dbg::printm(MODULE, "Cannot access disk %d, no disks\n", disk+1);
            std::abort();
        }
        uint8_t encounteredDisks = -1;
        for(auto blockDrivers : driver::getDrivers(driver::driverType::BLOCK)){
            assert(blockDrivers->getDeviceType() == driver::driverType::BLOCK);
            driver::MSCDriver* blockDriver = reinterpret_cast<driver::MSCDriver*>(blockDrivers);
            encounteredDisks += blockDriver->getConnectedDrives();
            if(encounteredDisks >= disk){
                std::pair<driver::MSCDriver*, uint8_t> ret;
                ret.first = blockDriver;
                ret.second = 0;
                dbg::popTrace();
                return ret;
            }
        }
        abort();
    }
    void readGPT(uint8_t disk){
        dbg::addTrace(__PRETTY_FUNCTION__); 
        auto drvDisk = translateVirtualDiskToPhysicalDisk(disk);
        driver::MSCDriver* blockDriver = drvDisk.first;
        dbg::printm(MODULE, "Using disk 0x%llx\n", blockDriver);
        uint8_t newDisk = drvDisk.second;
        PartitionTableHeader *PTH = new PartitionTableHeader;
        if(!blockDriver->read(newDisk, 1, 1, PTH)){
            dbg::printm(MODULE, "ERROR: Failed to read partition table header\n");
            std::abort();
        }
        uint8_t* buffer = new uint8_t[15872];
        if(!blockDriver->read(newDisk, 2, 31, buffer)){
            dbg::printm(MODULE, "ERROR: Failed to read partition entries\n");
            std::abort();
        }
        std::vector<PartitionEntry*> entries;
        for(uint32_t i = 0; i < PTH->partitionCount; i++){
            PartitionEntry* entry = (PartitionEntry*)(buffer+(i*sizeof(PartitionEntry)));
            if(entry->startLBA == 0 && entry->endLBA == 0){
                break;
            }
            if(entry->endLBA > blockDriver->getDiskSize(newDisk)){
                dbg::printm(MODULE, "ERROR: GPT entry more then maximum disk size\n");
                abort();
            }
            uint8_t* newGUID = parseGUID(entry->GUID);
            std::memcpy(entry->GUID, newGUID, sizeof(entry->GUID));
            PartitionEntry* acEntry = new PartitionEntry;
            std::memcpy(acEntry, entry, sizeof(PartitionEntry));
            dbg::printm(MODULE, "GPT Entry %d = %llx (%llu-%llu)\n", i, acEntry, acEntry->startLBA, acEntry->endLBA);
            entries.push_back(acEntry);
        }
        dbg::printm(MODULE, "Read GPT from disk %hhu with %llu entries\n", disk, entries.size());
        delete[] buffer;
        delete PTH;
        if(entries.size() == 0){
            dbg::printm(MODULE, "ERROR: Unable to find any partitions on disk %hd\n", disk);
            abort();
        }
        if((uint8_t)(disk+1) > partitionEntries.size()){
            partitionEntries.resize(disk+1);
        }
        partitionEntries[disk] = entries;
        dbg::popTrace();
    }
    void mount(uint8_t disk, uint8_t partition, const char* mountLocation){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        if(partitionEntries.size() == 0 || partitionEntries.at(disk).size() == 0){
            readGPT(disk);
        }
        if(partitionEntries.size() == 0 || partitionEntries.at(disk).size() == 0){
            dbg::printm(MODULE, "ERROR: Unable to read GPT from disk %hd\n", disk);
            abort();
        }
        dbg::printm(MODULE, "Mounting %d:%d to %s\n", disk, partition, mountLocation);
        if(partitionEntries.size() < disk){
            dbg::printm(MODULE, "ERROR: Unable to mount disk %hhd as it doesn't have a partition table\n");
            std::abort();
        }
        if(partitionEntries.at(disk).size() < partition){
            dbg::printm(MODULE, "ERROR: Partition is out of the possible partitions\n");
            dbg::printm(MODULE, "Attempted to load partition %hhd but only %lld partitions were found\n", partition, partitionEntries.at(disk).size());
            std::abort();
        }
        auto drvDisk = translateVirtualDiskToPhysicalDisk(disk);
        drivers::FSDriver* fileSystemdriver = drivers::loadFSDriver(partitionEntries.at(disk).at(partition), drvDisk);
        MountPoint* mp = new MountPoint;
        mp->fileSystemDriver = fileSystemdriver;
        mp->mountPath = mountLocation;
        mountPoints.push_back(mp);
        dbg::printm(MODULE, "Mounted %d:%d to %s\n", disk, partition, mountLocation);
        dbg::popTrace();
    }
    int openFile(const char* path, int flags){
        dbg::addTrace(__PRETTY_FUNCTION__);
        int handle = 0;
        for(MountPoint* mp : mountPoints){
            if(mp == nullptr || mp->fileSystemDriver == nullptr){
                continue;
            }
            if(std::memcmp(path, mp->mountPath, std::strlen(mp->mountPath)) == 0){
                path+=std::strlen(mp->mountPath);
                handle = mp->fileSystemDriver->open(task::getCurrentPID(), path, flags);
                dbg::printm(MODULE, "Opened file on %s with handle %d\n", mp->mountPath, handle);
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
        dbg::printm(MODULE, "TODO: Close files\n");
        abort();
        dbg::popTrace();
    }
    void printInfo(){
        dbg::printm(MODULE, "INFORMATION\n");
        dbg::printm(MODULE, "Mounted file systems: %llu\n", mountPoints.size());
        dbg::printm(MODULE, "Read partition tables: %llu\n", partitionEntries.size());
    }
};