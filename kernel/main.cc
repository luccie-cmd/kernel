#include <cstdio>
#include <kernel/vfs/vfs.h>
#include <common/dbg/dbg.h>

extern "C" void KernelMain(){
    dbg::addTrace(__PRETTY_FUNCTION__);
    if(!vfs::isInitialized()){
        vfs::initialize();
    }
    vfs::mount(0, 0, "/boot");
    vfs::mount(0, 1, "/");
    std::FILE* initFile = vfs::openFile(0, 0, "/boot/init.exe");
    for(;;);
}