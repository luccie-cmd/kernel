#include <drivers/display/vga/vga.h>
#include <common/dbg/dbg.h>
#include <kernel/mmu/vmm/vmm.h>
#include <common/io/io.h>
#define MODULE "VGA driver"
#define MASK_RED   0xFF000000
#define MASK_GREEN 0x00FF0000
#define MASK_BLUE  0x0000FF00
#define MASK_ALPHA 0x000000FF
#define VGA_IO_PORT 0x3D4
#define VGA_IO_DATA 0x3D5

namespace drivers::display{
    VgaDriver::VgaDriver() :DisplayDriver(DisplayType::Vga){
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->baseAddr = 0;
        dbg::popTrace();
    }
    VgaDriver::~VgaDriver(){}
    void VgaDriver::init(pci::device* dev){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Loading VGA display driver (0x%04x:0x%04x)\n", dev->vendorID, dev->deviceID);
        this->baseAddr = pci::readConfig(dev, 0x10)+mmu::vmm::getHHDM();
        for(size_t pciBar = 0x10; pciBar < 0x28; pciBar+=0x04){
            dbg::printm(MODULE, "PCI Bar %u = 0x%llx\n", (pciBar-0x10)/4, pci::readConfig(dev, pciBar));
        }
        dbg::printm(MODULE, "Loaded VGA display driver with width=%u height=%u\n", this->width, this->height);
        dbg::popTrace();
    }
    void VgaDriver::deinit(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::popTrace();
    }
    void VgaDriver::writePixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::popTrace();
    }
    void VgaDriver::writePixel(uint64_t x, uint64_t y, uint32_t rgba){
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->writePixel(x, y, rgba & MASK_RED, rgba & MASK_GREEN, rgba & MASK_BLUE, rgba & MASK_ALPHA);
        dbg::popTrace();
    }
    VgaDriver* loadVgaDriver(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        VgaDriver* drv = new VgaDriver;
        drv->init(device);
        dbg::popTrace();
        return drv;
    }
};