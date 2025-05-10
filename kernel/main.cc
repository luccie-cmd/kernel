#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/io/regs.h>
#include <cstdlib>
#include <cstring>
#include <drivers/display.h>
#include <drivers/input/kbd.h>
#include <kernel/acpi/acpi.h>
#include <kernel/hal/arch/init.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <kernel/vfs/vfs.h>

drivers::DisplayDriver* displayDriver;
extern void (*__init_array[])();
extern void (*__init_array_end[])();

void AbiCallCtors() {
    for (std::size_t i = 0; &__init_array[i] != __init_array_end; i++) {
        __init_array[i]();
    }
}

extern "C" void KernelMain() {
    hal::arch::earlyInit();
    dbg::addTrace(__PRETTY_FUNCTION__);
    AbiCallCtors();
    displayDriver = new drivers::DisplayDriver();
    hal::arch::midInit();
    drivers::DisplayDriver* temp = reinterpret_cast<drivers::DisplayDriver*>(
        driver::getDrivers(driver::driverType::DISPLAY).at(0));
    temp->setScreenX(displayDriver->getScreenX());
    temp->setScreenY(displayDriver->getScreenY());
    delete displayDriver;
    displayDriver = temp;
    vfs::mount(0, 0, "/boot");
    vfs::mount(0, 1, "/");
    vfs::createFile("/test.txt");
    int handle = vfs::openFile("/test.txt", 0);
    vfs::writeFile(handle, std::strlen("Hello, World12\nHello2\0"), "Hello, World12\nHello2\0");
    vfs::closeFile(handle);
    handle       = vfs::openFile("/test.txt", 0);
    char* buffer = new char[vfs::getLen(handle)];
    vfs::readFile(handle, vfs::getLen(handle), buffer);
    buffer[vfs::getLen(handle)] = '\0';
    dbg::printf("`%s`\n", buffer);
    delete[] buffer;
    drivers::input::kbd::KeyboardDriver* kbdDriver =
        reinterpret_cast<drivers::input::kbd::KeyboardDriver*>(
            driver::getDrivers(driver::driverType::INPUT).at(0));
    std::vector<drivers::input::kbd::KeyboardInputPair> buffer2 = kbdDriver->getKeyPresses(20);
    for (drivers::input::kbd::KeyboardInputPair b : buffer2) {
        dbg::printf("%s %s %s\n", b.data.c_str(), b.release ? "Released" : "Pressed",
                    b.special ? "Special" : "Normal");
    }
    // vfs::closeFile(handle);
    // vfs::umount("/");
    std::abort();
    for (;;);
}