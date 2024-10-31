#include <kernel/vfs/vfs.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#define MODULE "VFS"

namespace vfs{
    std::FILE* openFile(uint8_t disk, uint8_t partition, const char* path){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm("TODO: Open file\n", MODULE);
        std::abort();
        dbg::popTrace();
    }
};