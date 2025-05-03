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
    displayDriver->drawChar(0, 'E');
    while(1){
        __asm__("nop");
    }
    vfs::mount(0, 1, "/");
    vfs::createFile("/test.txt");
    int handle = vfs::openFile("/test.txt", 0);
    vfs::writeFile(handle, std::strlen("Hello, World12\nHello2\0"), "Hello, World12\nHello2\0");
    vfs::closeFile(handle);
    handle = vfs::openFile("/test.txt", 0);
    char *buffer = new char[vfs::getLen(handle)];
    vfs::readFile(handle, vfs::getLen(handle), buffer);
    buffer[vfs::getLen(handle)] = '\0';
    dbg::printf("`%s`\n", buffer);
    delete[] buffer;
    // vfs::closeFile(handle);
    // vfs::umount("/");
    std::abort();
    for (;;)
        ;
}