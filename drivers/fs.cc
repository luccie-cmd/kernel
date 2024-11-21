#include <stl/vector>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <utility>
#include <drivers/fs.h>
#include <kernel/vfs/vfs.h>
#define MODULE "File System driver"

namespace drivers {
    std::vector<std::pair<uint32_t, FSType>> GUIDEntries;
    FSDriver* loadFSDriver(vfs::PartitionEntry* entry, driver::MSCDriver* storageDriver) {
        dbg::addTrace(__PRETTY_FUNCTION__);
        GUIDEntries.clear();
        GUIDEntries.push_back(std::pair<uint32_t, FSType>(0xaf3dc60f, FSType::EXT4));
        GUIDEntries.push_back(std::pair<uint32_t, FSType>(0x28732ac1, FSType::FAT32));
        assert(entry);
        for (auto& guidEntry : GUIDEntries) {
            if (std::memcmp((void*)(uintptr_t)(&guidEntry.first), entry->GUID, sizeof(uint32_t)) == 0) {
                switch (guidEntry.second) {
                    case FSType::EXT4: {
                        dbg::printm(MODULE, "TODO: load EXT4 FS driver\n");
                        dbg::popTrace();
                        return nullptr;
                    }
                    case FSType::FAT32: {
                        dbg::printm(MODULE, "TODO: load FAT32 FS driver\n");
                        dbg::popTrace();
                        return nullptr;
                    }
                }
            }
        }

        dbg::printm(MODULE, "ERROR: Unable to find a matching FS for GUID: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
            entry->GUID[0], entry->GUID[1], entry->GUID[2], entry->GUID[3], entry->GUID[4], entry->GUID[5],
            entry->GUID[6], entry->GUID[7], entry->GUID[8], entry->GUID[9], entry->GUID[10], entry->GUID[11],
            entry->GUID[12], entry->GUID[13], entry->GUID[14], entry->GUID[15]);
        std::abort();
    }
};
