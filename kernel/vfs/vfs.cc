#include <kernel/vfs/vfs.h>
#include <kernel/vfs/gpt.h>
#include <common/dbg/dbg.h>
#include <kernel/driver/driver.h>
#include <drivers/msc.h>
#include <cstdlib>
#include <cstring>
#include <kernel/task/task.h>
#include <utility>
#include <cassert>
#define MODULE "VFS"
#define MAX_OPEN_FILES 128
#define MAX_MOUNTPOINTS 16

namespace vfs{
    bool inited = false;
    std::vector<std::vector<PartitionEntry*>> partitionEntries;
    MountPoint* mountPoints[MAX_MOUNTPOINTS];
    VFSFile* vfsFiles[MAX_OPEN_FILES];
    bool isInitialized(){
        return inited;
    }
    VFSFile* newVFSFile(int mpIdx, int fsHandle){
        for(size_t i = 0; MAX_OPEN_FILES; ++i){
            if(!vfsFiles[i]){
                vfsFiles[i] = new VFSFile;
            }
            if(vfsFiles[i]->used == false){
                vfsFiles[i]->used = true;
                vfsFiles[i]->vfsHandle = i;
                vfsFiles[i]->mpIdx = mpIdx;
                vfsFiles[i]->fsHandle = fsHandle;
                return vfsFiles[i];
            }
        }
        dbg::printm(MODULE, "No more VFS files available\n");
        abort();
    }
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        partitionEntries.clear();
        readGPT(0);
        inited = true;
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
    static std::pair<drivers::MSCDriver*, uint8_t> translateVirtualDiskToPhysicalDisk(uint8_t disk){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(driver::getDevicesCount(driver::driverType::BLOCK) == 0){
            dbg::printm(MODULE, "Cannot access disk %u, no disks\n", disk);
            std::abort();
        }
        uint8_t encounteredDisks = -1;
        for(auto blockDrivers : driver::getDrivers(driver::driverType::BLOCK)){
            assert(blockDrivers->getDeviceType() == driver::driverType::BLOCK);
            drivers::MSCDriver* blockDriver = reinterpret_cast<drivers::MSCDriver*>(blockDrivers);
            encounteredDisks += blockDriver->getConnectedDrives();
            if(encounteredDisks >= disk){
                std::pair<drivers::MSCDriver*, uint8_t> ret;
                ret.first = blockDriver;
                ret.second = 0;
                dbg::popTrace();
                return ret;
            }
        }
        dbg::printf("Unable to find disk %u\n", disk);
        std::abort();
    }
    void readGPT(uint8_t disk){
        dbg::addTrace(__PRETTY_FUNCTION__); 
        auto drvDisk = translateVirtualDiskToPhysicalDisk(disk);
        drivers::MSCDriver* blockDriver = drvDisk.first;
        uint8_t newDisk = drvDisk.second;
        PartitionTableHeader *PTH = new PartitionTableHeader;
        if(!blockDriver->read(newDisk, 1, 1, PTH)){
            dbg::printm(MODULE, "Failed to read partition table header\n");
            std::abort();
        }
        if (std::memcmp(PTH->signature, "EFI PART", 8) != 0){
            dbg::printm(MODULE, "Partition header corrupted got a signature of `%8s`\n", PTH->signature);
            std::abort();
        }
        uint8_t* buffer = new uint8_t[15872];
        if(!blockDriver->read(newDisk, 2, 31, buffer)){
            dbg::printm(MODULE, "Failed to read partition entries\n");
            std::abort();
        }
        std::vector<PartitionEntry*> entries;
        for(uint32_t i = 0; i < PTH->partitionCount; i++){
            PartitionEntry* entry = (PartitionEntry*)(buffer+(i*sizeof(PartitionEntry)));
            if(entry->startLBA == 0 && entry->endLBA == 0){
                break;
            }
            if(entry->endLBA > blockDriver->getDiskSize(newDisk)){
                dbg::printm(MODULE, "GPT entry more then maximum disk size\n");
                dbg::printf("NOTE: Got %llx but maximum is %llx\n", entry->endLBA, blockDriver->getDiskSize(newDisk));
                std::abort();
            }
            uint8_t* newGUID = parseGUID(entry->GUID);
            std::memcpy(entry->GUID, newGUID, sizeof(entry->GUID));
            PartitionEntry* acEntry = new PartitionEntry;
            std::memcpy(acEntry, entry, sizeof(PartitionEntry));
            entries.push_back(acEntry);
            dbg::printm(MODULE, "Loaded new partition: %llu-%llu\n", acEntry->startLBA, acEntry->endLBA);
        }
        delete[] buffer;
        delete PTH;
        if(entries.size() == 0){
            dbg::printm(MODULE, "Unable to find any partitions on disk %hd\n", disk);
            std::abort();
        }
        if((uint8_t)(disk+1) > partitionEntries.size()){
            partitionEntries.resize(disk+1);
        }
        partitionEntries[disk] = entries;
        dbg::popTrace();
    }
    std::pair<MountPoint*, size_t> findMountpoint(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        for(size_t i = 0; i < MAX_MOUNTPOINTS; ++i){
            MountPoint* mp = mountPoints[i];
            if(!mp){
                mp = new MountPoint;
                mp->mounted = false;
            }
            if(mp->mounted == false){
                dbg::popTrace();
                return {mp, i};
            }
        }
        dbg::printm(MODULE, "Could not find available mount point\n");
        std::abort();
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
            dbg::printm(MODULE, "Unable to read GPT from disk %hd\n", disk);
            abort();
        }
        if(partitionEntries.size() <= disk){
            dbg::printm(MODULE, "Unable to mount disk %hhd as it doesn't have a partition table\n");
            std::abort();
        }
        if(partitionEntries.at(disk).size() <= partition){
            dbg::printm(MODULE, "Partition is out of the possible partitions\n");
            dbg::printm(MODULE, "Attempted to load partition %hhd but only %lld partitions were found\n", partition, partitionEntries.at(disk).size());
            std::abort();
        }
        auto drvDisk = translateVirtualDiskToPhysicalDisk(disk);
        drivers::FSDriver* fileSystemdriver = drivers::loadFSDriver(partitionEntries.at(disk).at(partition), drvDisk);
        if(!fileSystemdriver){
            dbg::printm(MODULE, "Failed to load file system driver!!!\n");
            dbg::popTrace();
            return;
        }
        std::pair<MountPoint*, size_t> mp = findMountpoint();
        mp.first->fileSystemDriver = fileSystemdriver;
        mp.first->mountPath = mountLocation;
        mp.first->mounted = true;
        mountPoints[mp.second] = mp.first;
        dbg::printm(MODULE, "Successfully mounted %hhu:%hhu to %s\n", disk, partition, mountLocation);
        dbg::popTrace();
    }
    void umount(const char* path){
        for(size_t i = 0; i < MAX_MOUNTPOINTS; ++i){
            MountPoint* mp = mountPoints[i];
            if(!mp){
                continue;
            }
            if(mp->mounted == false || mp->fileSystemDriver == nullptr || mp->mountPath == nullptr){
                continue;
            }
            if(std::memcmp(path, mp->mountPath, std::strlen(mp->mountPath)) == 0){
                mp->mounted = false;
                delete mp->fileSystemDriver;
            }
        }
        dbg::printm(MODULE, "Successfully unmounted %s\n", path);
    }
    int openFile(const char* path, int flags){
        dbg::addTrace(__PRETTY_FUNCTION__);
        int handle = -1;
        int mpIdx = -1;
        const char* pathWithoutMountPoint;
        for(size_t i = 0; i < MAX_MOUNTPOINTS; ++i){
            MountPoint* mp = mountPoints[i];
            if(!mp){
                continue;
            }
            if(mp->mounted == false || mp->fileSystemDriver == nullptr || mp->mountPath == nullptr){
                continue;
            }
            if(std::memcmp(path, mp->mountPath, std::strlen(mp->mountPath)) == 0){
                const char* copyPath = path;
                copyPath+=std::strlen(mp->mountPath);
                pathWithoutMountPoint = copyPath;
                handle = mp->fileSystemDriver->open(task::getCurrentPID(), copyPath, flags);
                mpIdx = i;
                break;
            }
        }   
        if(handle == -1 || mpIdx == -1){
            dbg::printm(MODULE, "Unable to find file `%s`\n", path);
            std::abort();
        }
        VFSFile* vfsFile = newVFSFile(mpIdx, handle);
        vfsFile->pathWithoutMountPoint = pathWithoutMountPoint + 1;
        dbg::popTrace();
        return vfsFile->vfsHandle;
    }   
    void closeFile(int handle){
        dbg::addTrace(__PRETTY_FUNCTION__);
        VFSFile* vfsFile = vfsFiles[handle];
        if(!vfsFile){
            dbg::printm(MODULE, "Tried closing non existent entry!!!\n");
            abort();
        }
        if(vfsFile->used == false){
            dbg::printm(MODULE, "Tried closing already closed file!!!\n");
            abort();
        }
        if(vfsFile->mpIdx > MAX_MOUNTPOINTS){
            dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
            abort();
        }
        MountPoint* mp = mountPoints[vfsFile->mpIdx];
        mp->fileSystemDriver->close(vfsFile->fsHandle);
        vfsFile->used = false;
        dbg::popTrace();
    }
    void readFile(int handle, int size, void* buffer){
        dbg::addTrace(__PRETTY_FUNCTION__);
        VFSFile* vfsFile = vfsFiles[handle];
        if(!vfsFile){
            dbg::printm(MODULE, "Tried reading non existent entry!!!\n");
            abort();
        }
        if(vfsFile->mpIdx > MAX_MOUNTPOINTS){
            dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
            abort();
        }
        if(vfsFile->used == false){
            dbg::printm(MODULE, "Invalid use of already closed file\n");
            abort();
        }
        MountPoint* mp = mountPoints[vfsFile->mpIdx];
        mp->fileSystemDriver->read(vfsFile->fsHandle, size, buffer);
        dbg::popTrace();
    }
    void writeFile(int handle, int size, const void* buffer){
        dbg::addTrace(__PRETTY_FUNCTION__);
        VFSFile* vfsFile = vfsFiles[handle];
        if(!vfsFile){
            dbg::printm(MODULE, "Tried reading non existent entry!!!\n");
            abort();
        }
        if(vfsFile->mpIdx > MAX_MOUNTPOINTS){
            dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
            abort();
        }
        if(vfsFile->used == false){
            dbg::printm(MODULE, "Invalid use of already closed file\n");
            abort();
        }
        MountPoint* mp = mountPoints[vfsFile->mpIdx];
        mp->fileSystemDriver->write(vfsFile->fsHandle, size, buffer);
        dbg::popTrace();
    }
    int getLen(int handle){
        dbg::addTrace(__PRETTY_FUNCTION__);
        VFSFile* vfsFile = vfsFiles[handle];
        if(!vfsFile){
            dbg::printm(MODULE, "Tried reading non existent entry!!!\n");
            abort();
        }
        if(vfsFile->mpIdx > MAX_MOUNTPOINTS){
            dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
            abort();
        }
        if(vfsFile->used == false){
            dbg::printm(MODULE, "Invalid use of already closed file\n");
            abort();
        }
        MountPoint* mp = mountPoints[vfsFile->mpIdx];
        int size = mp->fileSystemDriver->getLengthOfFile(vfsFile->fsHandle);
        dbg::popTrace();
        return size;
    }
    void printInfo(){
        dbg::printm(MODULE, "INFO\n");
        for (MountPoint* mp : mountPoints){
            if (mp == nullptr){
                continue;
            }
            if (mp->mounted){
                dbg::printm(MODULE, "Mount point: %s ", mp->mountPath);
                switch (mp->fileSystemDriver->getFsType()){
                    case drivers::FSType::FAT32: {
                        dbg::print("FAT32\n");
                    } break;
                    case drivers::FSType::EXT2: {
                        dbg::print("EXT2\n");
                    } break;
                    case drivers::FSType::EXT3: {
                        dbg::print("EXT3\n");
                    } break;
                    case drivers::FSType::EXT4: {
                        dbg::print("EXT4\n");
                    } break;
                }
                for (int i = 0; i < MAX_OPEN_FILES; ++i){
                    VFSFile* vfsFile = vfsFiles[i];
                    if (vfsFile == nullptr){
                        continue;
                    }
                    if(mountPoints[vfsFile->mpIdx] == mp && vfsFile->used){
                        dbg::printf("\t- `%s`: %d (Size: %lu)\n", vfsFile->pathWithoutMountPoint, vfsFile->fsHandle, getLen(i));
                    }
                }
            }
        }
    }
};