#if !defined(_KERNEL_VFS_GPT_H_)
#define _KERNEL_VFS_GPT_H_
#include <cstdint>

namespace vfs{
    struct PartitionTableHeader{
        char signature[8];
        uint32_t revision;
        uint32_t headerSize;
        uint32_t CRC32;
        uint32_t reserved0;
        uint64_t currentLBA;
        uint64_t reserveLBA;
        uint64_t firstUseLBA;
        uint64_t lastUseLBA;
        char GUID[16];
        uint64_t firstPartitionEntry;
        uint32_t partitionCount;
        uint32_t partitionSize;
        uint32_t partitionCRC32;
        char reserved1[420];
    } __attribute__((packed));
    static_assert(sizeof(PartitionTableHeader) == 512, "Imporperly aligned partition table header");
};

#endif // _KERNEL_VFS_GPT_H_
