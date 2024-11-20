#include <drivers/fs.h>
#include <cassert>
#define MODULE "File System driver"

namespace drivers{
    FSDriver* loadFSDriver(vfs::PartitionEntry* entry){
        dbg::addTrace(__PRETTY_FUNCTION__);
        assert(entry);
        int fsType = entry->attr & VFS_PARTITION_ATTR_FSTYPE;
        dbg::printm(MODULE, "%d\n", fsType);
        dbg::popTrace();
        return nullptr;
    }
};