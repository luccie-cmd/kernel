#include <common/dbg/dbg.h>
#include <cstdlib>
#include <cstring>
#include <drivers/fs/sfs.h>
#include <string>
#define MODULE "SFS FS driver"

namespace drivers::fs
{
    SFSDriver::SFSDriver(vfs::PartitionEntry *entry, std::pair<MSCDriver *, uint8_t> drvDisk) : FSDriver(entry, drvDisk)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->__fs_type = FSType::SFS;
        this->superBlock = new SuperBlockBlock;
        if (!drvDisk.first->read(drvDisk.second, entry->startLBA, 1, this->superBlock))
        {
            dbg::printm(MODULE, "Couldn't read superblock\n");
            std::abort();
        }
        if (this->superBlock->header.type != SFSBlockTypes::SuperBlock)
        {
            dbg::printm(MODULE, "Corrupted filesystem, superblock invalid\n");
            std::abort();
        }
        this->rootDir = new DirectoryBlock;
        if (!drvDisk.first->read(drvDisk.second, this->superBlock->rootDirLBA, 1, this->rootDir))
        {
            dbg::printm(MODULE, "Couldn't read root directory\n");
            std::abort();
        }
        if (this->rootDir->header.type != SFSBlockTypes::Directory)
        {
            dbg::printm(MODULE, "Corrupted filesystem, rootdir invalid %hhu\n", this->rootDir->header.type);
            std::abort();
        }
        dbg::popTrace();
    }
    SFSDriver::~SFSDriver() {}
    void SFSDriver::init(pci::device *dev)
    {
        (void)dev;
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "File systems drivers shouldn't be initialized with PCI devices!!!\n");
        std::abort();
    }
    void SFSDriver::deinit()
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "File systems drivers shouldn't be deinitialized with PCI devices!!!\n");
        std::abort();
    }
    int SFSDriver::open(task::pid_t PID, const char *path, int flags)
    {
        (void)PID;
        (void)flags;
        dbg::addTrace(__PRETTY_FUNCTION__);
        if (path[0] == '/')
        {
            path++;
        }
        std::string strPath(path);
        DirectoryBlock *lastDirBlock = this->rootDir;
        while (true)
        {
            size_t slashPos = strPath.find('/');
            if (slashPos == std::string::npos)
                break;

            std::string dirName = strPath.substr(0, slashPos);
            strPath = strPath.substr(slashPos + 1);
            lastDirBlock = this->openDir(lastDirBlock, dirName.c_str());
            if (!lastDirBlock)
            {
                dbg::printm(MODULE, "Directory not found: %s\n", dirName.c_str());
                std::abort();
            }
        }
        uint64_t readLBA = lastDirBlock->header.currentLBA;
        while (readLBA)
        {
            for (uint32_t i = 0; i < lastDirBlock->blocksCount; ++i)
            {
                FileBlock *fileBlock = new FileBlock;
                if (!this->getDiskDevice().first->read(this->getDiskDevice().second, lastDirBlock->blocksLBA[i], 1, fileBlock))
                {
                    dbg::printm(MODULE, "Failed to read LBA %llu\n", lastDirBlock->blocksLBA[i]);
                    std::abort();
                }
                if (fileBlock->header.type != SFSBlockTypes::File)
                {
                    continue;
                }
                NameBlock *nameBlock = new NameBlock;
                if (!this->getDiskDevice().first->read(this->getDiskDevice().second, fileBlock->nameBlock, 1, nameBlock))
                {
                    dbg::printm(MODULE, "Failed to read LBA %llu\n", fileBlock->nameBlock);
                    std::abort();
                }
                const char *name = this->collectName(nameBlock);
                if (strcmp(strPath.c_str(), name) == 0)
                {
                    int handle = this->findHandle(lastDirBlock->blocksLBA[i]);
                    dbg::popTrace();
                    return handle;
                }
            }
            readLBA = lastDirBlock->nextDirBlock;
            if (!this->getDiskDevice().first->read(this->getDiskDevice().second, readLBA, 1, lastDirBlock))
            {
                dbg::printm(MODULE, "Failed to read LBA %llu\n", readLBA);
                std::abort();
            }
        }
        dbg::printm(MODULE, "No file exists with the name %s\n", strPath.c_str());
        dbg::popTrace();
        return -1;
    }
    void SFSDriver::read(int file, size_t length, void *buffer)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint64_t fileLBA = this->files.at(file)->lba;
        FileBlock *fileBlock = new FileBlock;
        if (!this->getDiskDevice().first->read(this->getDiskDevice().second, fileLBA, 1, fileBlock))
        {
            dbg::printm(MODULE, "Failed to read LBA %llu\n", fileLBA);
            std::abort();
        }
        DataBlock *dataBlock = new DataBlock;
        uint64_t readLBA = fileBlock->dataBlock;
        uint64_t bufferCount = 0;
        uint8_t* u8buffer = (uint8_t*)buffer;
        while (readLBA && length)
        {
            uint16_t count = 0;
            if (!this->getDiskDevice().first->read(this->getDiskDevice().second, readLBA, 1, dataBlock))
            {
                dbg::printm(MODULE, "Failed to read LBA %llu\n", readLBA);
                std::abort();
            }
            while(dataBlock->data[count] && count < sizeof(dataBlock->data) && length){
                u8buffer[bufferCount++] = dataBlock->data[count++];
                length--;
            }
            readLBA = dataBlock->nextData;
        }
    }
    void SFSDriver::write(int file, size_t length, const void *buffer)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint64_t fileLBA = this->files.at(file)->lba;
        FileBlock *fileBlock = new FileBlock;
        if (!this->getDiskDevice().first->read(this->getDiskDevice().second, fileLBA, 1, fileBlock))
        {
            dbg::printm(MODULE, "Failed to read LBA %llu\n", fileLBA);
            std::abort();
        }
        DataBlock *dataBlock = new DataBlock;
        uint64_t writeLBA = fileBlock->dataBlock;
        while (writeLBA)
        {
            if (!this->getDiskDevice().first->read(this->getDiskDevice().second, writeLBA, 1, dataBlock))
            {
                dbg::printm(MODULE, "Failed to read LBA %llu\n", writeLBA);
                std::abort();
            }
            writeLBA = dataBlock->nextData;
        }
        uint64_t c = 0;
        while (dataBlock->data[c])
        {
            c++;
        }
        if (c + length > sizeof(dataBlock->data))
        {
            dbg::printm(MODULE, "Split write block in 2 or more to fit it\n");
            std::abort();
        }
        const uint8_t *u8buffer = (const uint8_t *)buffer;
        size_t count = 0;
        while (count < length)
        {
            dataBlock->data[c] = u8buffer[count];
            c++;
            count++;
        }
        this->writeDataBlock(dataBlock);
    }
    void SFSDriver::close(int file)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->files.at(file)->opened = false;
        dbg::popTrace();
    }
    void SFSDriver::sync() {}
    uint64_t SFSDriver::getLengthOfFile(int file)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint64_t fileLBA = this->files.at(file)->lba;
        FileBlock *fileBlock = new FileBlock;
        if (!this->getDiskDevice().first->read(this->getDiskDevice().second, fileLBA, 1, fileBlock))
        {
            dbg::printm(MODULE, "Failed to read LBA %llu\n", fileLBA);
            std::abort();
        }
        uint64_t readLBA = fileBlock->dataBlock;
        uint64_t size = 0;
        while (readLBA)
        {
            uint16_t i = 0;
            DataBlock *dataBlock = new DataBlock;
            if (!this->getDiskDevice().first->read(this->getDiskDevice().second, readLBA, 1, dataBlock))
            {
                dbg::printm(MODULE, "Failed to read LBA %llu\n", readLBA);
                std::abort();
            }
            while (dataBlock->data[i])
            {
                size++;
                i++;
            }
            readLBA = dataBlock->nextData;
        }
        return size;
    }
    void SFSDriver::create(const char *path)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        if (path[0] == '/')
        {
            path++;
        }
        std::string strPath(path);
        DirectoryBlock *lastDirBlock = this->rootDir;
        while (true)
        {
            size_t slashPos = strPath.find('/');
            if (slashPos == std::string::npos)
                break;

            std::string dirName = strPath.substr(0, slashPos);
            strPath = strPath.substr(slashPos + 1);
            lastDirBlock = this->openDir(lastDirBlock, dirName.c_str());
            if (!lastDirBlock)
            {
                dbg::printm(MODULE, "Directory not found: %s\n", dirName.c_str());
                std::abort();
            }
        }
        FileBlock *fileBlock = new FileBlock;
        fileBlock->header.type = SFSBlockTypes::File;
        fileBlock->header.currentLBA = this->findFreeLBA();
        fileBlock->permissions = 0;
        NameBlock *nameBlock = new NameBlock;
        nameBlock->header.type = SFSBlockTypes::Name;
        nameBlock->header.currentLBA = this->findFreeLBA();
        nameBlock->nextName = 0;
        uint64_t copyStrPathLen = 0;
        while (copyStrPathLen < strPath.size())
        {
            nameBlock->characters[nameBlock->length++] = strPath[copyStrPathLen++];
            if (nameBlock->length >= sizeof(nameBlock->characters))
            {
                nameBlock->nextName = this->findFreeLBA();
                NameBlock *nextBlock = new NameBlock;
                nextBlock->header.type = SFSBlockTypes::Name;
                nextBlock->header.currentLBA = nameBlock->nextName;
                nextBlock->length = 0;
                nameBlock = nextBlock;
            }
        }
        DataBlock *dataBlock = new DataBlock;
        dataBlock->header.currentLBA = this->findFreeLBA();
        dataBlock->header.type = SFSBlockTypes::Data;
        dataBlock->nextData = 0;
        std::memset(dataBlock->data, 0, sizeof(dataBlock->data));
        fileBlock->nameBlock = nameBlock->header.currentLBA;
        fileBlock->dataBlock = dataBlock->header.currentLBA;
        this->writeNameBlock(nameBlock);
        this->writeDataBlock(dataBlock);
        this->writeFileBlock(lastDirBlock, fileBlock);
        dbg::popTrace();
    }
    void SFSDriver::writeNameBlock(NameBlock *nameBlock)
    {
        size_t writeLBA = nameBlock->header.currentLBA;
        while (writeLBA)
        {
            if (!this->getDiskDevice().first->write(this->getDiskDevice().second, nameBlock->header.currentLBA, 1, nameBlock))
            {
                dbg::printm(MODULE, "Failed to write LBA %llu\n", nameBlock->header.currentLBA);
                std::abort();
            }
            writeLBA = nameBlock->nextName;
        }
    }
    void SFSDriver::writeDataBlock(DataBlock *dataBlock)
    {
        size_t writeLBA = dataBlock->header.currentLBA;
        while (writeLBA)
        {
            if (!this->getDiskDevice().first->write(this->getDiskDevice().second, dataBlock->header.currentLBA, 1, dataBlock))
            {
                dbg::printm(MODULE, "Failed to write LBA %llu\n", dataBlock->header.currentLBA);
                std::abort();
            }
            writeLBA = dataBlock->nextData;
        }
    }
    void SFSDriver::writeFileBlock(DirectoryBlock *lastDirBlock, FileBlock *fileBlock)
    {
        if (!this->getDiskDevice().first->write(this->getDiskDevice().second, fileBlock->header.currentLBA, 1, fileBlock))
        {
            dbg::printm(MODULE, "Failed to write LBA %llu\n", fileBlock->header.currentLBA);
            std::abort();
        }
        lastDirBlock->blocksLBA[lastDirBlock->blocksCount++] = fileBlock->header.currentLBA;
        this->writeDirectoryBlock(lastDirBlock);
        constexpr uint32_t max_blocks_per_dir = (512 - 32) / 8;

        if (lastDirBlock->blocksCount >= max_blocks_per_dir)
        {
            DirectoryBlock *newDirBlock = new DirectoryBlock;
            newDirBlock->header.type = SFSBlockTypes::Directory;
            newDirBlock->header.currentLBA = this->findFreeLBA();
            newDirBlock->blocksCount = 0;
            newDirBlock->nextDirBlock = 0;

            lastDirBlock->nextDirBlock = newDirBlock->header.currentLBA;
            this->writeDirectoryBlock(lastDirBlock);
            lastDirBlock = newDirBlock;
        }
    }
    void SFSDriver::writeDirectoryBlock(DirectoryBlock *dirBlock)
    {
        if (!this->getDiskDevice().first->write(this->getDiskDevice().second, dirBlock->header.currentLBA, 1, dirBlock))
        {
            dbg::printm(MODULE, "Failed to write LBA %llu\n", dirBlock->header.currentLBA);
            std::abort();
        }
    }
    uint64_t SFSDriver::findFreeLBA()
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        size_t LBA = this->rootDir->header.currentLBA + 1;
        while (LBA < this->getPartEntry()->endLBA - this->getPartEntry()->startLBA)
        {
            uint8_t *buffer = new uint8_t[512];
            if (!this->getDiskDevice().first->read(this->getDiskDevice().second, LBA, 1, buffer))
            {
                dbg::printm(MODULE, "Failed to read LBA %llu\n", LBA);
                std::abort();
            }
            if (buffer[0] == 0)
            {
                buffer[0] = (uint8_t)SFSBlockTypes::Temp;
                if (!this->getDiskDevice().first->write(this->getDiskDevice().second, LBA, 1, buffer))
                {
                    dbg::printm(MODULE, "Failed to write LBA %llu\n", LBA);
                    std::abort();
                }
                delete[] buffer;
                dbg::popTrace();
                return LBA;
            }
            LBA++;
            delete[] buffer;
        }
        dbg::printm(MODULE, "No free LBA could be found\n");
        std::abort();
    }
    DirectoryBlock *SFSDriver::openDir(DirectoryBlock *current, const char *delim)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint64_t readLBA = current->header.currentLBA;
        while (readLBA)
        {
            for (uint32_t i = 0; i < current->blocksCount; ++i)
            {
                if (current->blocksLBA[i] == 0)
                {
                    continue;
                }
                DirectoryBlock *dirBlock = new DirectoryBlock;
                if (!this->getDiskDevice().first->read(this->getDiskDevice().second, current->blocksLBA[i], 1, dirBlock))
                {
                    dbg::printm(MODULE, "Failed to read LBA %llu\n", current->blocksLBA[i]);
                    std::abort();
                }
                if (dirBlock->header.type != SFSBlockTypes::Directory)
                {
                    delete dirBlock;
                    continue;
                }
                NameBlock *nameBlock = new NameBlock;
                if (!this->getDiskDevice().first->read(this->getDiskDevice().second, dirBlock->nameBlock, 1, dirBlock))
                {
                    dbg::printm(MODULE, "Failed to read LBA %llu\n", current->blocksLBA[i]);
                    std::abort();
                }
                const char *name = this->collectName(nameBlock);
                if (strcmp(name, delim) == 0)
                {
                    dbg::popTrace();
                    return dirBlock;
                }
                delete dirBlock;
            }
            readLBA = current->nextDirBlock;
            if (!this->getDiskDevice().first->read(this->getDiskDevice().second, readLBA, 1, current))
            {
                dbg::printm(MODULE, "Failed to read LBA %llu\n", readLBA);
                std::abort();
            }
        }
        dbg::popTrace();
        return nullptr;
    }
    const char *SFSDriver::collectName(NameBlock *nameBlock)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        std::string name;
        name.clear();
        uint64_t readLBA = nameBlock->header.currentLBA;
        while (readLBA)
        {
            name.reserve(name.size() + nameBlock->length);
            for (uint16_t i = 0; i < nameBlock->length; ++i)
            {
                name.push_back(nameBlock->characters[i]);
            }
            readLBA = nameBlock->nextName;
            if (!this->getDiskDevice().first->read(this->getDiskDevice().second, readLBA, 1, nameBlock))
            {
                dbg::printm(MODULE, "Failed to read LBA %llu\n", readLBA);
                std::abort();
            }
        }
        dbg::popTrace();
        return name.c_str();
    }
    int SFSDriver::findHandle(uint64_t lba)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        int handle = -1;
        for (int i = 0; i < (int)this->files.size(); ++i)
        {
            if (!this->files.at(i)->opened)
            {
                handle = i;
                break;
            }
        }
        if (handle == -1)
        {
            dbg::printm(MODULE, "Ran out of files, adding new one\n");
            SFSFile *f = new SFSFile;
            f->opened = false;
            this->files.push_back(f);
            for (int i = 0; i < (int)this->files.size(); ++i)
            {
                if (!this->files.at(i)->opened)
                {
                    handle = i;
                    break;
                }
            }
        }
        this->files.at(handle)->lba = lba;
        dbg::popTrace();
        return handle;
    }
};