#if !defined(_KERNEL_VFS_VFS_H_)
#define _KERNEL_VFS_VFS_H_
#include <cstdio>
#include <cstdint>

namespace vfs{
    bool isInitialized();
    void initialize();
    std::FILE* openFile(uint8_t disk, uint8_t partition, const char* path);
};

#endif // _KERNEL_VFS_VFS_H_
