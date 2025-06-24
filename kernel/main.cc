#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/io/regs.h>
#include <cstdlib>
#include <cstring>
#include <drivers/display.h>
#include <drivers/input/kbd.h>
#include <kernel/acpi/acpi.h>
#include <kernel/hal/arch/init.h>
#include <kernel/hal/arch/x64/gdt/gdt.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <kernel/mmu/mmu.h>
#include <kernel/objects/elf.h>
#include <kernel/task/task.h>
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
    displayDriver   = temp;
    task::pid_t pid = task::getNewPID();
    vfs::mount(0, 0, "/tmpboot");
    int                      handle  = vfs::openFile("/tmpboot/hello", 0);
    objects::elf::ElfObject* initObj = objects::elf::loadElfObject(handle);
    task::makeNewProcess(pid, initObj->entryPoint, handle, initObj->mappings);

    while (true) {
        task::nextProc();
    }

    std::abort();
    for (;;);
}