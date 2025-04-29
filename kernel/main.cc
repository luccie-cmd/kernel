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
    char *buffer = new char[vfs::getLen(handle)];
    vfs::readFile(handle, vfs::getLen(handle), (void *)buffer);
    // for (size_t i = 0; i < (size_t)vfs::getLen(handle); ++i){
        //     if (i % 16 == 0 && i != 0){
    //         dbg::print("\n");
    //     }
    //     dbg::printf("%02.2x ", buffer[i]);
    // }
    vfs::writeFile(handle, strlen("Test"), "Test");
    delete[] buffer;
    buffer = new char[vfs::getLen(handle)];
    vfs::readFile(handle, vfs::getLen(handle), (void *)buffer);
    dbg::printf("Test `%.*s`\n", vfs::getLen(handle), buffer);
    dbg::printf("Everything worked %d\n", handle);
    // vfs::closeFile(handle);
    // vfs::umount("/boot");
    std::abort();
    for (;;)
        ;
}