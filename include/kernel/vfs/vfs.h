#if !defined(_KERNEL_VFS_VFS_H_)
#define _KERNEL_VFS_VFS_H_
#include <cstdio>
#include <cstdint>
#include <drivers/fs.h>
#include <kernel/hal/pci/pci.h>

namespace vfs{
    struct MountPoint{
        const char* mountPath;
        drivers::FSDriver* fileSystemDriver;
        bool mounted;
    };
    struct VFSFile{
        uint64_t vfsHandle;
        uint64_t mpIdx;
        uint64_t fsHandle;
        bool used;
        const char* pathWithoutMountPoint;
    };
    uint8_t* parseGUID(uint8_t* GUID);
    bool isInitialized();
    void initialize();
    void readGPT(uint8_t disk);
    void mount(uint8_t disk, uint8_t partition, const char* mountLoc);
    void mount(const char* file, const char* mountLoc);
    void umount(const char* mountLocation);
    uint64_t openFile(const char* path, uint64_t flags);
    void closeFile(uint64_t handle);
    void readFile(uint64_t handle, uint64_t size, void* buffer);
    void writeFile(uint64_t handle, uint64_t size, const void* buffer);
    uint64_t getLen(uint64_t handle);
    void createFile(const char* path);
    void printInfo();
};

#endif // _KERNEL_VFS_VFS_H_
