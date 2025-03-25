#include <drivers/display.h>
#include <common/dbg/dbg.h>
#include <cassert>
#include <cstdlib>
#include <drivers/display/vga/vga.h>
#define MODULE "Display Driver Manager"

namespace drivers{
    DisplayDriver::DisplayDriver(DisplayType type) :Driver(driver::driverType::DISPLAY){
        this->__displayType = type;
    }
    DisplayDriver::~DisplayDriver(){}
    DisplayDriver* loadDisplayDriver(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        DisplayDriver* driver = nullptr;
        switch(device->subclassCode){
            case 0x00: {
                driver = display::loadVgaDriver(device);
            } break;
            default: {
                dbg::printm(MODULE, "TODO: Load display driver with subclass 0x%02x\n", device->subclassCode);
                std::exit(1);
            } break;
        }
        assert(driver != nullptr);
        dbg::popTrace();
        return driver;
    }
};