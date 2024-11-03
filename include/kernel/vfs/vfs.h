#if !defined(_KERNEL_VFS_VFS_H_)
#define _KERNEL_VFS_VFS_H_
#include <cstdio>
#include <cstdint>

namespace vfs{
    bool isInitialized();
    void initialize();
    void readGPT();
    void mount(uint8_t disk, uint8_t partition, const char* mountLoc);
    void mount(const char* file, const char* mountLoc);
    void umount(const char* mountLocation);
    std::FILE* openFile(const char* path);
};

#endif // _KERNEL_VFS_VFS_H_
