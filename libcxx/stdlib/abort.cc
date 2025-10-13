#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <drivers/display.h>
#include <kernel/acpi/acpi.h>
#include <kernel/driver/driver.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <kernel/mmu/mmu.h>
#include <kernel/vfs/vfs.h>
#include <vector>
extern drivers::DisplayDriver* displayDriver;
std::Spinlock                  abortLock;

extern "C" void abort() {
    abortLock.lock();
    dbg::print("ABORTING KERNEL\n");
    dbg::print("SENDING IPI\n");
    hal::arch::x64::irq::sendIPIs(0xFE);
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
    abortLock.unlock();
    while (1) {
        io::cli();
        __asm__("hlt");
    }
}