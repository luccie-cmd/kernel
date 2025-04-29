#include <array>
#include <cassert>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <cstring>
#include <drivers/fs.h>
#include <drivers/fs/ext4.h>
#include <drivers/fs/fat32.h>
#include <kernel/vfs/vfs.h>
#include <utility>
#define MODULE "File System driver"

namespace drivers
{
    FSDriver::FSDriver(vfs::PartitionEntry *entry, std::pair<MSCDriver *, uint8_t> drvDisk) : Driver(driver::driverType::FILESYSTEM)
    {
        this->__diskDevice = drvDisk;
        this->__partition_entry = entry;
    }
    FSDriver::~FSDriver() {}
    std::pair<MSCDriver *, uint8_t> FSDriver::getDiskDevice()
    {
        return this->__diskDevice;
    }
    vfs::PartitionEntry *FSDriver::getPartEntry()
    {
        return this->__partition_entry;
    }
    FSType FSDriver::getFsType()
    {
        return this->__fs_type;
    }
    static std::array<std::pair<uint32_t, FSType>, 2> GUIDEntries;
    FSDriver *loadFSDriver(vfs::PartitionEntry *entry, std::pair<MSCDriver *, uint8_t> drvDisk)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        GUIDEntries[0] = std::pair<uint32_t, FSType>(0xaf3dc60f, FSType::EXT4);
        GUIDEntries[1] = std::pair<uint32_t, FSType>(0x28732ac1, FSType::FAT32);
        assert(entry);
        FSDriver *drv = nullptr;
        for (auto guidEntry : GUIDEntries)
        {
            if (std::memcmp((void *)(uintptr_t)(&guidEntry.first), entry->GUID, sizeof(uint32_t)) == 0)
            {
                switch (guidEntry.second)
                {
                case FSType::EXT2:
                case FSType::EXT3:
                case FSType::EXT4:
                {
                    dbg::printm(MODULE, "TODO: load EXT FS driver\n");
                    dbg::popTrace();
                    return nullptr;
                }
                case FSType::FAT32:
                {
                    drv = new fs::FAT32Driver(entry, drvDisk);
                }
                break;
                }
            }
        }
        if (drv)
        {
            dbg::popTrace();
            return drv;
        }
        dbg::printm(MODULE, "Unable to find a matching FS for GUID: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                    entry->GUID[0], entry->GUID[1], entry->GUID[2], entry->GUID[3], entry->GUID[4], entry->GUID[5],
                    entry->GUID[6], entry->GUID[7], entry->GUID[8], entry->GUID[9], entry->GUID[10], entry->GUID[11],
                    entry->GUID[12], entry->GUID[13], entry->GUID[14], entry->GUID[15]);
        std::abort();
    }
};
