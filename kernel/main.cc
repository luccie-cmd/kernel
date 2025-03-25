#include <cstdlib>
#include <kernel/vfs/vfs.h>
#include <common/dbg/dbg.h>
#include <kernel/hal/arch/init.h>
#include <cstring>

extern "C" void KernelMain(){
    hal::arch::earlyInit();
    dbg::addTrace(__PRETTY_FUNCTION__);
    vfs::mount(0, 0, "/tmp");
    int handle = vfs::openFile("/tmp/limine.conf", 0);
    dbg::printf("Everything worked %d\n", handle);
    char *buffer = new char[vfs::getLen(handle)];
    
    vfs::readFile(handle, vfs::getLen(handle), (void*)buffer);
    vfs::closeFile(handle);
    std::abort();
    for(;;);
}