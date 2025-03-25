#include <drivers/fs/fat32.h>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include <kernel/task/task.h>
#include <common/dbg/dbg.h>
#include <cassert>
#include <string>
#define MODULE "FAT32 Driver"

namespace drivers::fs{
    FAT32Driver::FAT32Driver(vfs::PartitionEntry* entry, std::pair<MSCDriver*, uint8_t> drvDisk) :FSDriver(entry, drvDisk){
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
        if(!drvDisk.first->read(drvDisk.second, rootDirLba+entry->startLBA, 1, this->rootDir->Buffer)){
            dbg::printm(MODULE, "Failed to read root directory buffer!!!\n");
            std::abort();
        }
        this->files.clear();
        this->files.resize(0);
        dbg::popTrace();
    }
    FAT32Driver::~FAT32Driver(){
        delete this->rootDir;
        for(FAT_FileData* data : this->files){
            delete data;
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
                name[len] = '\0';
                path += len;
                isLast = true;
            }
            FAT_DirectoryEntry entry;
            if(this->findFile(current, name, &entry)){
                if (!isLast && ((entry.Attributes & (uint8_t)FAT_Attributes::DIRECTORY) == 0)){
                    dbg::printm(MODULE, "%s not a directory\r\n", name);
                    this->close(current->Handle);
                    dbg::popTrace();
                    return -1;
                }
                this->close(current->Handle);
                current = this->openEntry(&entry);
            } else{
                dbg::printm(MODULE, "%s couldn't be found\n", name);
                this->close(current->Handle);
                dbg::popTrace();
                return -1;
            }
        }
        dbg::popTrace();
        return current->Handle;
    }
    void FAT32Driver::read(int file, size_t length, void* buffer){
        dbg::addTrace(__PRETTY_FUNCTION__);
        FAT_File fatFile = this->files.at(file)->Public;
        this->readBytes(&fatFile, fatFile.Size, buffer);
        dbg::popTrace();
    }
    void FAT32Driver::close(int file){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(file > (int)this->files.size()){
            dbg::printm(MODULE, "ERROR: Can't close file which was not opened by FAT (handle=%llu)\n", file);
            std::abort();
        }
        if(file == FAT32_ROOT_DIRECTORY_HANDLE){
            this->rootDir->Public.Position = 0;
            this->rootDir->CurrentCluster = this->rootDir->FirstCluster;
        } else{
            this->files.at(file)->Opened = false;
        }
        dbg::popTrace();
    }
    int FAT32Driver::getLengthOfFile(int file){
        dbg::addTrace(__PRETTY_FUNCTION__);
        FAT_File fatFile = this->files.at(file)->Public;
        int size = fatFile.Size;
        dbg::popTrace();
        return size;
    }
    uint32_t FAT32Driver::clusterToLBA(uint32_t cluster){
        return (this->dataSectionLBA + (cluster - 2) * this->bootSector->SectorsPerCluster);
    }
    static void getShortName(char* name, char shortName[12]) {
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
        FAT_FileData* fd = (file->Handle == FAT32_ROOT_DIRECTORY_HANDLE ? this->rootDir : this->files.at(file->Handle));
        uint8_t* u8DataOut = (uint8_t*)dataOut;
        if(u8DataOut == nullptr){
            dbg::printm(MODULE, "ERROR: Unable to write to nullptr\n");
            std::abort();
        }
        uint32_t oldBytesCount = bytesCount;
        if (!fd->Public.IsDirectory || (fd->Public.IsDirectory && fd->Public.Size != 0)) {
            bytesCount = std::min(bytesCount, fd->Public.Size - fd->Public.Position);
        }
        if(bytesCount != oldBytesCount){
            dbg::printm(MODULE, "WARNING: Attempted to read more bytes then the file has left, overriding from %llu to %llu\n", oldBytesCount, bytesCount);
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
    static void appendLFN(FAT_DirectoryEntry* entry, std::vector<uint16_t>& buffer){
        const FAT_LFNEntry* lfn = reinterpret_cast<const FAT_LFNEntry*>(entry);
        if (lfn->Attributes != 0x0F) {
            dbg::printm(MODULE, "Error: Not an LFN entry\n");
            return;
        }
        uint16_t namePart[13];
        std::memcpy(&namePart[0], lfn->Name1, 5 * sizeof(uint16_t));
        std::memcpy(&namePart[5], lfn->Name2, 6 * sizeof(uint16_t));
        std::memcpy(&namePart[11], lfn->Name3, 2 * sizeof(uint16_t));
        for (uint16_t c : namePart) {
            if (c == 0xFFFF || c == 0x0000) break;
            buffer.push_back(c);
        }
    }
    static std::string decodeLFN(const std::vector<uint16_t>& buffer) {
        std::string result;
        for (uint16_t c : buffer) {
            if (c == 0xFFFF || c == 0x0000) break;
            result += static_cast<char>(c);
        }
        return result;
    }
    bool FAT32Driver::findFile(FAT_File* file, char* name, FAT_DirectoryEntry* outEntry){
        dbg::addTrace(__PRETTY_FUNCTION__);
        char shortName[12];
        FAT_DirectoryEntry *entry = new FAT_DirectoryEntry;
        assert(entry != nullptr);
        getShortName(name, shortName);
        std::vector<uint16_t> lfnBuffer;
        bool lfnActive = false;
        while(this->readEntry(file, entry)){
            if(entry->Name[0] == 0x00){
                break;
            }
            if(entry->Attributes == (uint8_t)FAT_Attributes::LFN){
                lfnActive = true;
                appendLFN(entry, lfnBuffer);
                continue;
            }
            if(lfnActive){
                std::string lfnName = decodeLFN(lfnBuffer);
                if(lfnName == name){
                    *outEntry = *entry;
                    dbg::popTrace();
                    return true;
                }
                lfnBuffer.clear();
                lfnActive = false;
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
        for (uint64_t i = 0; i < files.size(); ++i) {
            if(this->files.at(i) == nullptr){
                dbg::printm(MODULE, "ERROR: File at index %llu deallocated before module release\n", i);
                std::abort();
            }
            if (this->files.at(i)->Opened == false) {
                handle = i;
                break;
            }
        }
        if (handle == -1) {
            dbg::printm(MODULE, "WARNING: Ran out of fat file datas, adding new one\n");
            FAT_FileData* fd = new FAT_FileData;
            if(fd == nullptr){
                dbg::printm(MODULE, "ERROR: new FAT_FileData returned nullptr");
                std::abort();
            }
            fd->Opened = false;
            this->files.push_back(fd);
            for (uint64_t i = 0; i < this->files.size(); ++i) {
                if(this->files.data() == nullptr){
                    dbg::printm(MODULE, "ERROR: FAT files data integrety comprimised\n");
                    std::abort();
                }
                if(this->files.at(i) == nullptr){
                    dbg::printm(MODULE, "ERROR: File at index %llu deallocated before module release\n", i);
                    std::abort();
                }
                if (this->files.at(i)->Opened == false) {
                    handle = i;
                    break;
                }
            }
            if (handle == -1) {
                dbg::printm(MODULE, "No available file handles\n");
                std::abort();
            }
        }
        FAT_FileData* fd = this->files.at(handle);
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