#if !defined(_DRIVERS_FS_SFS_H_)
#define _DRIVERS_FS_SFS_H_
#include <drivers/fs.h>
#include <unordered_map>
#define SECTOR_SIZE 512

namespace drivers::fs {
enum struct SFSBlockTypes : uint8_t {
    Empty = 0,
    SuperBlock,
    Directory,
    File,
    Name,
    Data,
    Temp,
};
struct __attribute__((packed)) SFSBlockHeader {
    SFSBlockTypes type;
    uint64_t      currentLBA;
};
struct __attribute__((packed)) SuperBlockBlock {
    SFSBlockHeader header;
    uint64_t       rootDirLBA;
    uint8_t padding[495];
};
static_assert(sizeof(SuperBlockBlock) == 512, "SuperBlockBlock alignment is messed up");
struct __attribute__((packed)) DirectoryBlock {
    SFSBlockHeader header;
    uint64_t       nextDirBlock;
    uint64_t       nameBlock;
    uint32_t       blocksCount;
    uint64_t       blocksLBA[60];
    uint8_t padding[3];
};
static_assert(sizeof(DirectoryBlock) == 512, "DirectoryBlock alignment is messed up");
struct __attribute__((packed)) FileBlock {
    SFSBlockHeader header;
    uint64_t       nameBlock;
    uint64_t       dataBlock;
    uint32_t       permissions;
    uint8_t padding[483];
};
static_assert(sizeof(FileBlock) == 512, "FileBlock alignment is messed up");
struct __attribute__((packed)) NameBlock {
    SFSBlockHeader header;
    uint64_t       nextName;
    uint16_t       length;
    uint8_t        characters[493];
};
static_assert(sizeof(NameBlock) == 512, "NameBlock alignment is messed up");
struct __attribute__((packed)) DataBlock {
    SFSBlockHeader header;
    uint64_t       nextData;
    uint8_t        data[495];
};
static_assert(sizeof(DataBlock) == 512, "DataBlock alignment is messed up");
struct SFSFile {
    bool     opened;
    uint64_t lba;
    uint64_t position;
};
class SFSDriver : public FSDriver {
  public:
    SFSDriver(vfs::PartitionEntry* entry, std::pair<MSCDriver*, uint8_t> drvDisk);
    ~SFSDriver();
    void     init(pci::device* dev);
    void     deinit();
    int      open(task::pid_t PID, const char* path, int flags);
    void     read(int file, size_t length, void* buffer);
    void     write(int file, size_t length, const void* buffer);
    void     close(int file);
    void     sync();
    void     seek(int file, uint64_t offset);
    uint64_t getOffsetInFile(int file);
    uint64_t getLengthOfFile(int file);
    void     create(const char* path);

  private:
    void                  createDir(const char* basePath, const char* dirName);
    NameBlock*            readNameBlock(uint64_t nameBlock);
    void                  writeNameBlock(NameBlock* nameBlock);
    void                  writeDataBlock(DataBlock* dataBlock);
    void                  writeDirectoryBlock(DirectoryBlock* dirBlock);
    void                  writeFileBlock(DirectoryBlock* lastDirBlock, FileBlock* fileBlock);
    uint64_t              findFreeLBA();
    DirectoryBlock*       openDir(DirectoryBlock* current, const char* delim);
    const char*           collectName(NameBlock* nameBlock);
    int                   findHandle(uint64_t lba);
    SuperBlockBlock*      superBlock;
    DirectoryBlock*       rootDir;
    std::vector<SFSFile*> files;

    void ls(const char* path);
};
}; // namespace drivers::fs

#endif // _DRIVERS_FS_SFS_H_