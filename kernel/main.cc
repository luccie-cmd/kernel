#include <cstdio>
#include <kernel/vfs/vfs.h>
#include <common/dbg/dbg.h>

extern "C" void KernelMain(){
    dbg::addTrace(__PRETTY_FUNCTION__);
    if(!vfs::isInitialized()){
        vfs::initialize();
    }
    std::FILE* initFile = vfs::openFile(0, 0, "/init.exe");
    for(;;);
}