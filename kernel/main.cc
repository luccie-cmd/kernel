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

extern "C" void changeRSP(uint64_t newRSP, uint64_t size);

extern "C" std::vector<std::pair<void*, const char*>> funcAddrTable;

// void populateFuncAddrTable(){
//     uint64_t mapHandle = vfs::openFile("/tmpboot/kernel.map", 0);
//     if (mapHandle == -1){
//         dbg::printf("WARNING: No debugging symbols found, not attempting to populate
//         address-function table.\n"); return;
//     }
//     vfs::seek(mapHandle, 0);
//     uint64_t size = vfs::getLen(mapHandle);
// }

extern "C" void KernelMain() {
    hal::arch::earlyInit();
    dbg::addTrace(__PRETTY_FUNCTION__);
    uint64_t newRSP = mmu::pmm::allocVirtual(64 * PAGE_SIZE) + mmu::vmm::getHHDM();
    changeRSP(newRSP, 64 * PAGE_SIZE - 16);
    AbiCallCtors();
    if (!vfs::mount(0, 0, "/tmpboot")) {
        dbg::printf("ERROR WHILE LOADING INIT (Unable to mount boot partition)\n");
        std::abort();
    }
    // populateFuncAddrTable();
    displayDriver = new drivers::DisplayDriver();
    hal::arch::midInit();
    drivers::DisplayDriver* temp = reinterpret_cast<drivers::DisplayDriver*>(
        driver::getDrivers(driver::driverType::DISPLAY).at(0));
    temp->setScreenX(displayDriver->getScreenX());
    temp->setScreenY(displayDriver->getScreenY());
    delete displayDriver;
    displayDriver = temp;
    if (!vfs::mount(0, 1, "/")) {
        dbg::printf("ERROR WHILE LOADING INIT (Unable to mount root partition)\n");
        std::abort();
    }
    int handle = vfs::openFile("/tmpboot/init", 0);
    if (handle == -1) {
        dbg::printf("ERROR WHILE LOADING INIT (No init binary found)\n");
        std::abort();
    }
    objects::elf::ElfObject* initObj = objects::elf::loadElfObject(handle, 0);
    if (initObj == nullptr) {
        dbg::printf("ERROR WHILE LOADING INIT (Corrupted ELF binary)\n");
        std::abort();
    }
    task::pid_t pid = task::getNewPID();
    task::makeNewProcess(pid, initObj->entryPoint, handle, initObj->mappings,
                         initObj->dynamicSection);

    while (true) {
        task::nextProc();
    }

    std::abort();
    for (;;);
}