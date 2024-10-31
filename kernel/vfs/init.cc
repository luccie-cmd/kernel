#include <kernel/vfs/vfs.h>
#include <common/dbg/dbg.h>
#define MODULE "VFS"

namespace vfs{
    bool inited = false;
    bool isInitialized(){
        return inited;
    }
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm("Initializing...\n", MODULE);
        dbg::popTrace();
    }
};