#include <cassert>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <cstring>
#include <drivers/msc.h>
#include <kernel/driver/driver.h>
#include <kernel/task/task.h>
#include <kernel/vfs/gpt.h>
#include <kernel/vfs/vfs.h>
#include <utility>
#define MODULE "VFS"

namespace vfs {
bool                                      inited = false;
std::vector<std::vector<PartitionEntry*>> partitionEntries;
std::vector<MountPoint*>                  mountPoints;
std::vector<VFSFile*>                     vfsFiles;
bool                                      isInitialized() {
    return inited;
}
VFSFile* newVFSFile(uint64_t mpIdx, uint64_t fsHandle) {
    for (size_t i = 0; i < vfsFiles.size(); ++i) {
        VFSFile* file = vfsFiles.at(i);
        if (file->used == false) {
            file->used      = true;
            file->vfsHandle = i;
            file->mpIdx     = mpIdx;
            file->fsHandle  = fsHandle;
            return file;
        }
    }
    dbg::printm(MODULE, "Ran out of VFS files, adding new one\n");
    VFSFile* file = new VFSFile;
    vfsFiles.push_back(file);
    file->used      = true;
    file->mpIdx     = mpIdx;
    file->fsHandle  = fsHandle;
    file->vfsHandle = vfsFiles.size() - 1;
    return file;
}
void initialize() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    partitionEntries.clear();
    readGPT(0);
    inited = true;
    dbg::popTrace();
}
uint8_t* parseGUID(uint8_t* GUID) {
    uint8_t* newGUID = new uint8_t[16];
    newGUID[0]       = GUID[3];
    newGUID[1]       = GUID[2];
    newGUID[2]       = GUID[1];
    newGUID[3]       = GUID[0];
    newGUID[4]       = GUID[5];
    newGUID[5]       = GUID[4];
    newGUID[6]       = GUID[7];
    newGUID[7]       = GUID[6];
    newGUID[8]       = GUID[8];
    newGUID[9]       = GUID[9];
    newGUID[10]      = GUID[10];
    newGUID[11]      = GUID[11];
    newGUID[12]      = GUID[12];
    newGUID[13]      = GUID[13];
    newGUID[14]      = GUID[14];
    newGUID[15]      = GUID[15];
    return newGUID;
}
static std::pair<drivers::MSCDriver*, uint8_t> translateVirtualDiskToPhysicalDisk(uint8_t disk) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (driver::getDevicesCount(driver::driverType::BLOCK) == 0) {
        dbg::printm(MODULE, "Cannot access disk %u, no disks\n", disk);
        std::abort();
    }
    uint8_t encounteredDisks = 0;
    for (auto blockDrivers : driver::getDrivers(driver::driverType::BLOCK)) {
        assert(blockDrivers->getDeviceType() == driver::driverType::BLOCK);
        drivers::MSCDriver* blockDriver = reinterpret_cast<drivers::MSCDriver*>(blockDrivers);
        encounteredDisks += blockDriver->getConnectedDrives();
        if (encounteredDisks > disk) {
            std::pair<drivers::MSCDriver*, uint8_t> ret;
            ret.first  = blockDriver;
            ret.second = 0;
            dbg::popTrace();
            return ret;
        }
    }
    dbg::printf("Unable to find disk %u\n", disk);
    std::abort();
}
void readGPT(uint8_t disk) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    std::pair<drivers::MSCDriver*, uint8_t> drvDisk     = translateVirtualDiskToPhysicalDisk(disk);
    drivers::MSCDriver*                     blockDriver = drvDisk.first;
    uint8_t                                 newDisk     = drvDisk.second;
    PartitionTableHeader*                   PTH         = new PartitionTableHeader;
    if (!blockDriver->read(newDisk, 1, 1, (volatile uint8_t*)PTH)) {
        dbg::printm(MODULE, "Failed to read partition table header\n");
        std::abort();
    }
    if (std::memcmp(PTH->signature, "EFI PART", 8) != 0) {
        dbg::printm(MODULE, "Partition header corrupted got a signature of `%8s`\n",
                    PTH->signature);
        std::abort();
    }
    uint8_t* buffer = new uint8_t[15872];
    if (!blockDriver->read(newDisk, 2, 31, (volatile uint8_t*)buffer)) {
        dbg::printm(MODULE, "Failed to read partition entries\n");
        std::abort();
    }
    std::vector<PartitionEntry*> entries;
    for (uint32_t i = 0; i < PTH->partitionCount; i++) {
        PartitionEntry* entry = (PartitionEntry*)(buffer + (i * sizeof(PartitionEntry)));
        if (entry->startLBA == 0 && entry->endLBA == 0) {
            break;
        }
        if (entry->endLBA > blockDriver->getDiskSize(newDisk)) {
            dbg::printm(MODULE, "GPT entry more then maximum disk size\n");
            dbg::printf("NOTE: Got %llx but maximum is %llx\n", entry->endLBA,
                        blockDriver->getDiskSize(newDisk));
            std::abort();
        }
        uint8_t* newGUID = parseGUID(entry->GUID);
        std::memcpy(entry->GUID, newGUID, sizeof(entry->GUID));
        PartitionEntry* acEntry = new PartitionEntry;
        std::memcpy(acEntry, entry, sizeof(PartitionEntry));
        entries.push_back(acEntry);
        dbg::printm(MODULE, "Loaded new partition: %llu-%llu\n", acEntry->startLBA,
                    acEntry->endLBA);
    }
    delete[] buffer;
    delete PTH;
    if (entries.size() == 0) {
        dbg::printm(MODULE, "Unable to find any partitions on disk %hd\n", disk);
        std::abort();
    }
    if ((uint8_t)(disk + 1) > partitionEntries.size()) {
        partitionEntries.resize(disk + 1);
    }
    partitionEntries.at(disk) = entries;
    dbg::popTrace();
}
std::pair<MountPoint*, size_t> findMountpoint() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    for (size_t i = 0; i < mountPoints.size(); ++i) {
        MountPoint* mp = mountPoints.at(i);
        if (mp->mounted == false) {
            dbg::popTrace();
            return {mp, i};
        }
    }
    dbg::printm(MODULE, "Ran out of possible mount points, adding new one\n");
    MountPoint* mp = new MountPoint;
    mp->mounted    = false;
    mountPoints.push_back(mp);
    dbg::popTrace();
    return {mp, mountPoints.size() - 1};
}
bool mount(uint8_t disk, uint8_t partition, const char* mountLocation) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    if (partitionEntries.size() == 0 || partitionEntries.at(disk).size() == 0) {
        readGPT(disk);
    }
    if (partitionEntries.size() == 0 || partitionEntries.at(disk).size() == 0) {
        dbg::printm(MODULE, "Unable to read GPT from disk %hd\n", disk);
        std::abort();
    }
    if (partitionEntries.size() <= disk) {
        dbg::printm(MODULE, "Unable to mount disk %hhd as it doesn't have a partition table\n");
        std::abort();
    }
    if (partitionEntries.at(disk).size() <= partition) {
        dbg::printm(MODULE, "Partition is out of the possible partitions\n");
        dbg::printm(MODULE,
                    "Attempted to load partition %hhd but only %lld partitions were found\n",
                    partition, partitionEntries.at(disk).size());
        return false;
    }
    auto               drvDisk = translateVirtualDiskToPhysicalDisk(disk);
    drivers::FSDriver* fileSystemdriver =
        drivers::loadFSDriver(partitionEntries.at(disk).at(partition), drvDisk);
    if (!fileSystemdriver) {
        dbg::printm(MODULE, "Failed to load file system driver!!!\n");
        dbg::popTrace();
        return false;
    }
    std::pair<MountPoint*, size_t> mp = findMountpoint();
    mp.first->fileSystemDriver        = fileSystemdriver;
    mp.first->mountPath               = mountLocation;
    if (mp.first->mountPath[std::strlen(mp.first->mountPath) - 1] != '/') {
        char* newMountPath = new char[strlen(mountLocation) + 2];
        std::memcpy(newMountPath, mountLocation, std::strlen(mountLocation));
        newMountPath[std::strlen(mountLocation)] = '/';
        newMountPath[strlen(mountLocation) + 1]  = '\0';
        mp.first->mountPath                      = newMountPath;
    }
    mp.first->mounted         = true;
    mountPoints.at(mp.second) = mp.first;
    dbg::printm(MODULE, "Successfully mounted %hhu:%hhu to %s\n", disk, partition, mountLocation);
    dbg::popTrace();
    return true;
}
bool umount(const char* path) {
    bool unmounted = false;
    for (MountPoint* mp : mountPoints) {
        if (mp->mounted == false || mp->fileSystemDriver == nullptr || mp->mountPath == nullptr) {
            continue;
        }
        if (std::memcmp(path, mp->mountPath, std::strlen(mp->mountPath)) == 0) {
            mp->mounted = false;
            unmounted   = true;
            delete mp->fileSystemDriver;
            break;
        }
    }
    dbg::printm(MODULE, "%s unmounted %s\n", unmounted ? "Successfully" : "Unsuccessfully", path);
    return unmounted;
}
uint64_t openFile(const char* path, uint64_t flags) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    uint64_t    handle = static_cast<uint64_t>(-1);
    uint64_t    mpIdx  = static_cast<uint64_t>(-1);
    const char* pathWithoutMountPoint;
    for (size_t i = 0; i < mountPoints.size(); ++i) {
        MountPoint* mp = mountPoints.at(i);
        if (mp->mounted == false || mp->fileSystemDriver == nullptr || mp->mountPath == nullptr) {
            continue;
        }
        if (std::memcmp(path, mp->mountPath, std::strlen(mp->mountPath)) == 0) {
            const char* copyPath = path;
            copyPath += std::strlen(mp->mountPath);
            pathWithoutMountPoint = copyPath;
            handle                = mp->fileSystemDriver->open(KERNEL_PID, copyPath, flags);
            mpIdx                 = i;
            break;
        }
    }
    if (handle == static_cast<uint64_t>(-1) || mpIdx == static_cast<uint64_t>(-1)) {
        dbg::popTrace();
        return -1;
    }
    VFSFile* vfsFile               = newVFSFile(mpIdx, handle);
    vfsFile->pathWithoutMountPoint = pathWithoutMountPoint;
    dbg::popTrace();
    return vfsFile->vfsHandle;
}
uint64_t getOffset(uint64_t handle) {
    VFSFile* vfsFile = vfsFiles.at(handle);
    if (vfsFile->used == false) {
        dbg::printm(MODULE, "Tried closing already closed file!!!\n");
        std::abort();
    }
    if (vfsFile->mpIdx > mountPoints.size()) {
        dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
        std::abort();
    }
    MountPoint* mp = mountPoints.at(vfsFile->mpIdx);
    return mp->fileSystemDriver->getOffsetInFile(vfsFile->fsHandle);
}
void seek(uint64_t handle, uint64_t offset) {
    VFSFile* vfsFile = vfsFiles.at(handle);
    if (vfsFile->used == false) {
        dbg::printm(MODULE, "Tried closing already closed file!!!\n");
        std::abort();
    }
    if (vfsFile->mpIdx > mountPoints.size()) {
        dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
        std::abort();
    }
    MountPoint* mp = mountPoints.at(vfsFile->mpIdx);
    mp->fileSystemDriver->seek(vfsFile->fsHandle, offset);
}
void closeFile(uint64_t handle) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    VFSFile* vfsFile = vfsFiles.at(handle);
    if (vfsFile->used == false) {
        dbg::printm(MODULE, "Tried closing already closed file!!!\n");
        std::abort();
    }
    if (vfsFile->mpIdx > mountPoints.size()) {
        dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
        std::abort();
    }
    MountPoint* mp = mountPoints.at(vfsFile->mpIdx);
    mp->fileSystemDriver->sync();
    mp->fileSystemDriver->close(vfsFile->fsHandle);
    vfsFile->used = false;
    dbg::popTrace();
}
void createFile(const char* path) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    for (MountPoint* mp : mountPoints) {
        if (mp->mounted == false || mp->fileSystemDriver == nullptr || mp->mountPath == nullptr) {
            continue;
        }
        if (std::memcmp(path, mp->mountPath, std::strlen(mp->mountPath)) == 0) {
            const char* copyPath = path;
            copyPath += std::strlen(mp->mountPath);
            mp->fileSystemDriver->create(copyPath);
            dbg::popTrace();
            return;
        }
    }
    dbg::printm(MODULE, "Cannot create file %s\n", path);
    std::abort();
}
void readFile(uint64_t handle, uint64_t size, void* buffer) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    VFSFile* vfsFile = vfsFiles.at(handle);
    if (vfsFile->mpIdx > mountPoints.size()) {
        dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
        std::abort();
    }
    if (vfsFile->used == false) {
        dbg::printm(MODULE, "Invalid use of already closed file\n");
        std::abort();
    }
    if (size == 0) {
        size = vfs::getLen(handle);
    }
    MountPoint* mp = mountPoints.at(vfsFile->mpIdx);
    mp->fileSystemDriver->read(vfsFile->fsHandle, size, buffer);
    dbg::popTrace();
}
void writeFile(uint64_t handle, uint64_t size, const void* buffer) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    VFSFile* vfsFile = vfsFiles.at(handle);
    if (vfsFile->mpIdx > mountPoints.size()) {
        dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
        std::abort();
    }
    if (vfsFile->used == false) {
        dbg::printm(MODULE, "Invalid use of already closed file\n");
        std::abort();
    }
    if (size == 0) {
        dbg::printm(MODULE, "Attempted 0 size write\n");
        std::abort();
    }
    MountPoint* mp = mountPoints.at(vfsFile->mpIdx);
    mp->fileSystemDriver->write(vfsFile->fsHandle, size, buffer);
    mp->fileSystemDriver->sync();
    dbg::popTrace();
}
uint64_t getLen(uint64_t handle) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    VFSFile* vfsFile = vfsFiles.at(handle);
    if (vfsFile->mpIdx > mountPoints.size()) {
        dbg::printm(MODULE, "No mountpoint available at index 0x%llx\n", vfsFile->mpIdx);
        std::abort();
    }
    if (vfsFile->used == false) {
        dbg::printm(MODULE, "Invalid use of already closed file\n");
        std::abort();
    }
    MountPoint* mp   = mountPoints.at(vfsFile->mpIdx);
    uint64_t    size = mp->fileSystemDriver->getLengthOfFile(vfsFile->fsHandle);
    dbg::popTrace();
    return size;
}
void printInfo() {
    dbg::printm(MODULE, "INFO\n");
    for (MountPoint* mp : mountPoints) {
        if (mp->mounted) {
            dbg::printm(MODULE, "Mount point: %s ", mp->mountPath);
            switch (mp->fileSystemDriver->getFsType()) {
            case drivers::FSType::FAT32: {
                dbg::print("FAT32\n");
            } break;
            case drivers::FSType::SFS: {
                dbg::print("SFS\n");
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
            for (size_t i = 0; i < vfsFiles.size(); ++i) {
                VFSFile* vfsFile = vfsFiles.at(i);
                if (mountPoints.at(vfsFile->mpIdx) == mp && vfsFile->used) {
                    dbg::printf("\t- `%s`: %d (Size: %lu)\n", vfsFile->pathWithoutMountPoint,
                                vfsFile->fsHandle, getLen(i));
                }
            }
        }
    }
}
}; // namespace vfs