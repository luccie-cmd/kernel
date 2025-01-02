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
    };
    uint8_t* parseGUID(uint8_t* GUID);
    bool isInitialized();
    void initialize();
    void readGPT(uint8_t disk);
    void mount(uint8_t disk, uint8_t partition, const char* mountLoc);
    void mount(const char* file, const char* mountLoc);
    void umount(const char* mountLocation);
    int openFile(const char* path, int flags);
    void closeFile(int handle);
    void printInfo();
};

#endif // _KERNEL_VFS_VFS_H_
