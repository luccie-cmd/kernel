#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <drivers/display.h>
#include <kernel/acpi/acpi.h>
#include <kernel/driver/driver.h>
#include <kernel/mmu/mmu.h>
#include <kernel/vfs/vfs.h>
#include <vector>
extern drivers::DisplayDriver* displayDriver;

extern "C" void abort() {
    dbg::print("ABORTING KERNEL\n");
    drivers::DisplayDriver* temp = displayDriver;
    displayDriver                = nullptr;
    dbg::print("INFO:\n");
    vfs::printInfo();
    mmu::printInfo();
    driver::printInfo();
    acpi::printInfo();
    displayDriver = temp;
    dbg::print("STACK TRACE:\n");
    dbg::printStackTrace();
    while (1) {
        // io::cli();
        __asm__("hlt");
    }
}