#if !defined(_DRIVERS_FS_SFS_H_)
#define _DRIVERS_FS_SFS_H_
#include <drivers/fs.h>
#include <unordered_map>
#define SECTOR_SIZE 512

namespace drivers::fs
{
    enum struct SFSBlockTypes : uint8_t
    {
        Empty = 0,
        SuperBlock,
        Directory,
        File,
        Name,
        Data,
        Temp,
    };
    struct __attribute__((packed)) SFSBlockHeader
    {
        SFSBlockTypes type;
        uint64_t currentLBA;
    };
    struct __attribute__((packed)) __attribute__((aligned(SECTOR_SIZE))) SuperBlockBlock
    {
        SFSBlockHeader header;
        uint64_t rootDirLBA;
    };
    static_assert(sizeof(SuperBlockBlock) == 512, "SuperBlockBlock alignment is messed up");
    struct __attribute__((packed)) __attribute__((aligned(SECTOR_SIZE))) DirectoryBlock
    {
        SFSBlockHeader header;
        uint64_t nextDirBlock;
        uint64_t nameBlock;
        uint32_t blocksCount;
        uint64_t blocksLBA[];
    };
    static_assert(sizeof(DirectoryBlock) == 512, "DirectoryBlock alignment is messed up");
    struct __attribute__((packed)) __attribute__((aligned(SECTOR_SIZE))) FileBlock
    {
        SFSBlockHeader header;
        uint64_t nameBlock;
        uint64_t dataBlock;
        uint32_t permissions;
    };
    static_assert(sizeof(FileBlock) == 512, "FileBlock alignment is messed up");
    struct __attribute__((packed)) __attribute__((aligned(SECTOR_SIZE))) NameBlock
    {
        SFSBlockHeader header;
        uint64_t nextName;
        uint16_t length;
        uint8_t characters[493];
    };
    static_assert(sizeof(NameBlock) == 512, "NameBlock alignment is messed up");
    struct __attribute__((packed)) __attribute__((aligned(SECTOR_SIZE))) DataBlock
    {
        SFSBlockHeader header;
        uint64_t nextData;
        uint8_t data[495];
    };
    static_assert(sizeof(DataBlock) == 512, "DataBlock alignment is messed up");
    struct SFSFile{
        bool opened;
        uint64_t lba;
    };
    class SFSDriver : public FSDriver
    {
    public:
        SFSDriver(vfs::PartitionEntry *entry, std::pair<MSCDriver *, uint8_t> drvDisk);
        ~SFSDriver();
        void init(pci::device *dev);
        void deinit();
        int open(task::pid_t PID, const char *path, int flags);
        void read(int file, size_t length, void *buffer);
        void write(int file, size_t length, const void *buffer);
        void close(int file);
        void sync();
        uint64_t getLengthOfFile(int file);
        void create(const char *path);

    private:
        void writeNameBlock(NameBlock* nameBlock);
        void writeDataBlock(DataBlock* dataBlock);
        void writeDirectoryBlock(DirectoryBlock* dirBlock);
        void writeFileBlock(DirectoryBlock* lastDirBlock, FileBlock* fileBlock);
        uint64_t findFreeLBA();
        DirectoryBlock *openDir(DirectoryBlock* current, const char *delim);
        const char* collectName(NameBlock* nameBlock);
        int findHandle(uint64_t lba);
        SuperBlockBlock *superBlock;
        DirectoryBlock *rootDir;
        std::vector<SFSFile*> files;
    };
};

#endif // _DRIVERS_FS_SFS_H_