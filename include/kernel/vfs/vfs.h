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
        int vfsHandle;
        int mpIdx;
        int fsHandle;
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
    int openFile(const char* path, int flags);
    void closeFile(int handle);
    void readFile(int handle, int size, void* buffer);
    void writeFile(int handle, int size, const void* buffer);
    int getLen(int handle);
    void printInfo();
};

#endif // _KERNEL_VFS_VFS_H_
