#include <drivers/fs/fat32.h>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <kernel/task/task.h>
#include <common/dbg/dbg.h>
#define MODULE "FAT32 Driver"

namespace drivers::fs{
    FAT32Driver::FAT32Driver(vfs::PartitionEntry* entry, std::pair<driver::MSCDriver*, uint8_t> drvDisk) :FSDriver(entry, drvDisk){
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->bootSector = new FAT_BootSector;
        if(!drvDisk.first->read(drvDisk.second, entry->startLBA, 1, this->bootSector)){
            dbg::printm(MODULE, "Failed to read boot sector!!!\n");
            std::abort();
        }
        if((this->bootSector->reserved0[420] != 0x55 && this->bootSector->reserved0[421] != 0xaa)){
            dbg::printm(MODULE, "Failed to get correct bootsector %02x%02x%02x %02x%02x\n", this->bootSector->BootJumpInstruction[0], this->bootSector->BootJumpInstruction[1], this->bootSector->BootJumpInstruction[2], this->bootSector->reserved0[420], this->bootSector->reserved0[421]);
            std::abort();
        }
        std::memset(cache, 0, sizeof(cache));
        this->cachePos = 0xFFFFFFFF;
        this->maxSectors = this->bootSector->LargeSectorCount;
        this->sectorsPerFat = this->bootSector->EBR32.SectorsPerFat;
        this->dataSectionLBA = (this->bootSector->ReservedSectors + this->sectorsPerFat * this->bootSector->FatCount);
        uint32_t rootDirLba = clusterToLBA((this->bootSector->EBR32.RootDirectoryCluster == 0 ? 2 : this->bootSector->EBR32.RootDirectoryCluster));
        this->rootDir = new FAT_FileData;
        this->rootDir->Public.Handle = FAT32_ROOT_DIRECTORY_HANDLE;
        this->rootDir->Public.IsDirectory = true;
        this->rootDir->Public.Position = 0;
        this->rootDir->Public.Pid = KERNEL_PID;
        this->rootDir->Public.Size = sizeof(FAT_DirectoryEntry) * this->bootSector->DirEntryCount;
        this->rootDir->Opened = true;
        this->rootDir->FirstCluster = rootDirLba;
        this->rootDir->CurrentCluster = rootDirLba;
        this->rootDir->CurrentSectorInCluster = 0;
        drvDisk.first->read(drvDisk.second, rootDirLba+entry->startLBA, 1, this->rootDir->Buffer);
        dbg::print("\n");
        for(uint64_t i = 1; i < FAT32_MAX_FILE_HANDLES; ++i){
            if(!this->openedFiles[i]){
                this->openedFiles[i] = new FAT_FileData;
            }
            this->openedFiles[i]->Opened = false;
        }
        dbg::popTrace();
    }
    FAT32Driver::~FAT32Driver(){
        delete this->rootDir;
        for(uint64_t i = 0; i < FAT32_MAX_FILE_HANDLES; ++i){
            delete this->openedFiles[i];
        }
    }
    void FAT32Driver::init(pci::device* dev){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "File systems drivers shouldn't be initialized with PCI devices!!!\n");
        std::abort();
        dbg::popTrace();
    }
    void FAT32Driver::deinit(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "File systems drivers shouldn't be deinitialized with PCI devices!!!\n");
        std::abort();
        dbg::popTrace();
    }
    int FAT32Driver::open(task::pid_t PID, const char* path, int flags){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(path[0] == '/'){
            path++;
        }
        char name[256];
        FAT_File* current = &this->rootDir->Public;
        while(*path){
            bool isLast = false;
            const char* delim = std::strchr(path, '/');
            if(delim != nullptr){
                std::memcpy(name, path, delim - path);
                name[delim - path] = '\0';
                path = delim + 1;
            } else{
                size_t len = std::strlen(path);
                std::memcpy(name, path, len);
                name[len + 1] = '\0';
                path += len;
                isLast = true;
            }
            FAT_DirectoryEntry entry;
            if(this->findFile(current, name, &entry)){
                this->close(current->Handle);
                if (!isLast && ((entry.Attributes & (uint8_t)FAT_Attributes::DIRECTORY) == 0)){
                    dbg::printm(MODULE, "%s not a directory\r\n", name);
                    dbg::popTrace();
                    return 0;
                }
                current = this->openEntry(&entry);
            } else{
                this->close(current->Handle);
                dbg::printm(MODULE, "%s couldn't be found\n", name);
                dbg::popTrace();
                return 0;
            }
        }
        dbg::popTrace();
        return current->Handle;
    }
    void FAT32Driver::read(int file, size_t length, void* buffer){
        dbg::addTrace(__PRETTY_FUNCTION__);
        std::abort();
        dbg::popTrace();
    }
    void FAT32Driver::close(int file){
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->openedFiles[file]->Opened = false;
        dbg::popTrace();
    }
    uint32_t FAT32Driver::clusterToLBA(uint32_t cluster){
        return (this->dataSectionLBA + (cluster - 2) * this->bootSector->SectorsPerCluster);
    }
    static void getShortName(char* name, char shortName[12]){
        memset(shortName, ' ', 12);
        shortName[11] = '\0';
        const char* ext = std::strchr(name, '.');
        if (ext == NULL)
            ext = name + 11;

        for (int i = 0; i < 8 && name[i] && name + i < ext; i++)
            shortName[i] = std::toupper(name[i]);

        if (ext != name + 11){
            for (int i = 0; i < 3 && ext[i + 1]; i++)
                shortName[i + 8] = std::toupper(ext[i + 1]);
        }
    }
    uint32_t FAT32Driver::readBytes(FAT_File* file, uint32_t bytesCount, void* dataOut) {
        dbg::addTrace(__PRETTY_FUNCTION__);
        FAT_FileData* fd = (file->Handle == FAT32_ROOT_DIRECTORY_HANDLE ? this->rootDir : this->openedFiles[file->Handle]);
        uint8_t* u8DataOut = (uint8_t*)dataOut;
        if (!fd->Public.IsDirectory || (fd->Public.IsDirectory && fd->Public.Size != 0)) {
            bytesCount = std::min(bytesCount, fd->Public.Size - fd->Public.Position);
        }
        while (bytesCount > 0) {
            uint32_t leftInBuffer = SECTOR_SIZE - (fd->Public.Position % SECTOR_SIZE);
            uint32_t take = std::min(bytesCount, leftInBuffer);
            std::memcpy(u8DataOut, fd->Buffer + fd->Public.Position % SECTOR_SIZE, take);
            u8DataOut += take;
            fd->Public.Position += take;
            bytesCount -= take;
            if (leftInBuffer == take){
                if (fd->Public.Handle == FAT32_ROOT_DIRECTORY_HANDLE) {
                    fd->CurrentCluster++;
                    this->getDiskDevice().first->read(this->getDiskDevice().second, fd->CurrentCluster, 1, fd->Buffer);
                } else {
                    if (++fd->CurrentSectorInCluster >= this->bootSector->SectorsPerCluster) {
                        fd->CurrentSectorInCluster = 0;
                        fd->CurrentCluster = this->nextCluster(fd->CurrentCluster);
                    }
                    if (fd->CurrentCluster >= 0xFFFFFFF8) {
                        fd->Public.Size = fd->Public.Position;
                        dbg::printm(MODULE, "End of cluster chain at cluster=%u\n", fd->CurrentCluster);
                        break;
                    }
                    this->getDiskDevice().first->read(this->getDiskDevice().second, (this->clusterToLBA(fd->CurrentCluster) + fd->CurrentSectorInCluster)+this->getPartEntry()->startLBA, 1, fd->Buffer);
                }
            }
        }

        dbg::popTrace();
        return (u8DataOut - (uint8_t*)dataOut);
    }
    void FAT32Driver::readFat(size_t lbaIdx){
        size_t fatOffset = this->bootSector->ReservedSectors + lbaIdx;
        dbg::printm(MODULE, "Reading FAT table at LBA=%u\n", fatOffset);
        this->getDiskDevice().first->read(this->getDiskDevice().second, fatOffset, FAT32_CACHE_SIZE, this->cache);
    }
    uint32_t FAT32Driver::nextCluster(uint32_t currentCluster) {
        uint32_t fatIdx = currentCluster * 4;
        uint32_t fatIdxSector = fatIdx / SECTOR_SIZE;
        if (fatIdxSector < this->cachePos || fatIdxSector >= this->cachePos + FAT32_CACHE_SIZE) {
            this->readFat(fatIdxSector);
            this->cachePos = fatIdxSector;
        }
        fatIdx -= (this->cachePos * SECTOR_SIZE);
        uint32_t nextCluster = *(uint32_t*)(this->cache + fatIdx);
        dbg::printm(MODULE, "nextCluster: current=%u, next=%u\n", currentCluster, nextCluster);
        return nextCluster;
    }
    bool FAT32Driver::readEntry(FAT_File* file, FAT_DirectoryEntry* dirEntry){
        return this->readBytes(file, sizeof(FAT_DirectoryEntry), (void*)dirEntry) == sizeof(FAT_DirectoryEntry);
    }
    bool FAT32Driver::findFile(FAT_File* file, char* name, FAT_DirectoryEntry* outEntry){
        dbg::addTrace(__PRETTY_FUNCTION__);
        char shortName[12];
        FAT_DirectoryEntry *entry = new FAT_DirectoryEntry;
        getShortName(name, shortName);
        while(this->readEntry(file, entry)){
            if(entry->Name[0] == 0x00){
                dbg::popTrace();
                return false;
            }
            if(entry->Name[0] == ' '){
                dbg::popTrace();
                return false;
            }
            if(std::memcmp(shortName, entry->Name, 11) == 0){
                *outEntry = *entry;
                dbg::popTrace();
                return true;
            }
        }
        dbg::popTrace();
        return false;
    }
    FAT_File* FAT32Driver::openEntry(FAT_DirectoryEntry *entry) {
        dbg::addTrace(__PRETTY_FUNCTION__);
        int handle = -1;
        for (uint64_t i = 1; i < FAT32_MAX_FILE_HANDLES; ++i) {
            if (!this->openedFiles[i]->Opened) {
                handle = i;
                break;
            }
        }
        if (handle == -1) {
            dbg::printm(MODULE, "No available file handles\n");
            std::abort();
        }
        FAT_FileData* fd = this->openedFiles[handle];
        fd->Public.Handle = handle;
        fd->Public.IsDirectory = (entry->Attributes & (uint8_t)FAT_Attributes::DIRECTORY) != 0;
        fd->Public.Size = entry->Size;
        fd->Public.Position = 0;
        fd->Public.Pid = task::getCurrentPID();
        fd->Opened = true;
        fd->FirstCluster = (entry->FirstClusterHigh << 16) | entry->FirstClusterLow;
        fd->CurrentCluster = fd->FirstCluster;
        fd->CurrentSectorInCluster = 0;
        this->getDiskDevice().first->read(this->getDiskDevice().second, this->clusterToLBA(fd->FirstCluster)+this->getPartEntry()->startLBA, 1, fd->Buffer);
        dbg::popTrace();
        return &fd->Public;
    }

};