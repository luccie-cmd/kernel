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

uint8_t buffer[] = {0xCC, 0xCC, 0xEB, 0xFE};

extern "C" void     switchProc(uint64_t rsp, uint64_t rip, uint64_t cr3);
extern "C" uint64_t __trampoline_text_start;
extern "C" uint64_t __trampoline_text_end;
extern "C" uint64_t __trampoline_data_start;
extern "C" uint64_t __trampoline_data_end;

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
    task::makeNewProcess(pid);
    uint64_t physicalAddr = mmu::pmm::allocate();
    uint64_t virtualAddr  = 0x401000;
    mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), physicalAddr, virtualAddr,
                      PROTECTION_RW | PROTECTION_KERNEL, MAP_GLOBAL | MAP_PRESENT);
    std::memcpy(reinterpret_cast<void*>(virtualAddr), buffer, sizeof(buffer));
    // mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), virtualAddr);
    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), physicalAddr, virtualAddr, PROTECTION_RW,
                      MAP_GLOBAL | MAP_PRESENT);
    hal::arch::x64::gdt::setRSP0(pid);
    uint64_t userRSP = mmu::pmm::allocate();
    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), userRSP, userRSP - PAGE_SIZE, PROTECTION_RW,
                      MAP_GLOBAL | MAP_PRESENT);
    uint64_t trampoline_va = (uint64_t)&__trampoline_text_start;
    uint64_t trampoline_phys =
        mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
    while (trampoline_va < (uint64_t)&__trampoline_text_end) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), trampoline_phys, trampoline_va, PROTECTION_RW,
                          MAP_GLOBAL | MAP_PRESENT);
        trampoline_va += PAGE_SIZE;
        trampoline_phys += PAGE_SIZE;
    }
    trampoline_va = (uint64_t)&__trampoline_data_start;
    trampoline_phys =
        mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), trampoline_va, false);
    while (trampoline_va < (uint64_t)&__trampoline_data_end) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(pid), trampoline_phys, trampoline_va, PROTECTION_RW,
                          MAP_GLOBAL | MAP_PRESENT);
        trampoline_va += PAGE_SIZE;
        trampoline_phys += PAGE_SIZE;
    }

    switchProc(userRSP, virtualAddr,
               reinterpret_cast<uint64_t>(mmu::vmm::getPML4(pid)) - mmu::vmm::getHHDM());
    task::runProc(pid);
    // vfs::mount(0, 0, "/tmpboot");
    // int      handle = vfs::openFile("/tmpboot/init", 0);
    // uint8_t* buffer = new uint8_t[vfs::getLen(handle)];
    // dbg::printf("Init length: %llu init buffer address 0x%llx\n", vfs::getLen(handle), buffer);
    // vfs::umount("/tmpboot");

    std::abort();
    for (;;);
}