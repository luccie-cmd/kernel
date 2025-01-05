#include <cstdlib>
#include <kernel/vfs/vfs.h>
#include <common/dbg/dbg.h>
#include <kernel/hal/arch/init.h>

extern "C" void KernelMain(){
    dbg::addTrace(__PRETTY_FUNCTION__);
    hal::arch::earlyInit();
    vfs::mount(0, 1, "/");
    vfs::mount(0, 0, "/boot");
    int initFileHandle = vfs::openFile("/boot/test.txt", FS_FLAGS_READ);
    dbg::printf("Init file handle: %d\n", initFileHandle);
    vfs::closeFile(initFileHandle);
    dbg::printf("ERROR: Initialization program returned!!!\n");
    vfs::umount("/boot");
    vfs::umount("/");
    std::abort();
    for(;;);
}