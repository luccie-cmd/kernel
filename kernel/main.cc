#include <cstdio>
#include <kernel/vfs/vfs.h>
#include <common/dbg/dbg.h>
#include <kernel/hal/arch/init.h>

extern "C" void KernelMain(){
    hal::arch::earlyInit();
    dbg::addTrace(__PRETTY_FUNCTION__);
    if(!vfs::isInitialized()){
        vfs::initialize();
    }
    vfs::mount(0, 1, "/");
    vfs::mount(0, 0, "/boot");
    int initFileHandle = vfs::openFile("/boot/test.txt", FS_FLAGS_READ);
    dbg::printf("Init file handle: %d\n", initFileHandle);
    for(;;);
}