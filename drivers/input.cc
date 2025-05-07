#include <common/dbg/dbg.h>
#include <drivers/input.h>
#include <drivers/input/kbd/ps2.h>
#include <kernel/hal/pci/pci.h>
#define MODULE "Input driver manager"

namespace drivers::input
{
InputDriver::InputDriver(InputType type) : driver::Driver(driver::driverType::INPUT)
{
    this->__inputType = type;
}
InputDriver::~InputDriver() {}
std::vector<InputDriver*> loadInputDrivers()
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    std::vector<pci::device*> pciDevices   = pci::getAllDevices();
    bool                      kbdFound     = false;
    std::vector<InputDriver*> inputDrivers = {};
    for (pci::device* device : pciDevices)
    {
        if (device->classCode != 0x0c)
        {
            continue;
        }
        if (device->subclassCode != 0x03)
        {
            dbg::printm(MODULE, "TODO: Support non USB SBC devices\n");
            // std::abort();
        }
        dbg::printm(MODULE, "TODO: Support USB SBC devices\n");
        // std::abort();
    }
    if (!kbdFound)
    {
        inputDrivers.push_back(kbd::loadPS2Driver());
    }
    dbg::popTrace();
    return inputDrivers;
}
}; // namespace drivers::input