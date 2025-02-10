#include <cstdlib>
#include <kernel/vfs/vfs.h>
#include <common/dbg/dbg.h>
#include <kernel/hal/arch/init.h>

extern "C" void KernelMain(){
    hal::arch::earlyInit();
    dbg::addTrace(__PRETTY_FUNCTION__);
    vfs::mount(0, 0, "/");
    int handle = vfs::openFile("/", 0);
    dbg::printf("Everything worked %d\n", handle);
    vfs::closeFile(handle);
    std::abort();
    for(;;);
}