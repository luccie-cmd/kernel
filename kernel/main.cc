#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <cstring>
#include <drivers/display.h>
#include <kernel/hal/arch/init.h>
#include <kernel/vfs/vfs.h>

extern drivers::DisplayDriver *displayDriver;

extern "C" void KernelMain()
{
    hal::arch::earlyInit();
    dbg::addTrace(__PRETTY_FUNCTION__);
    displayDriver = reinterpret_cast<drivers::DisplayDriver *>(driver::getDrivers(driver::driverType::DISPLAY).at(0));
    vfs::mount(0, 0, "/boot");
    int handle = vfs::openFile("/boot/test.txt", 0);
    dbg::printf("Everything worked %d\n", handle);
    char *buffer = new char[vfs::getLen(handle)];
    vfs::readFile(handle, vfs::getLen(handle)-1, (void *)buffer);
    dbg::printf("Test `%.*s`\n", vfs::getLen(handle), buffer);
    vfs::closeFile(handle);
    vfs::umount("/boot");
    std::abort();
    for (;;)
        ;
}