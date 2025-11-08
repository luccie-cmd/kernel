#include <common/dbg/dbg.h>
#include <drivers/input.h>
#include <drivers/input/kbd/ps2.h>
#include <kernel/hal/pci/pci.h>
#define MODULE "Input driver manager"

namespace drivers::input {
InputDriver::InputDriver(InputType type) : driver::Driver(driver::driverType::INPUT) {
    this->__inputType = type;
}
InputDriver::~InputDriver() {}
std::vector<InputDriver*> loadInputDrivers() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    std::vector<InputDriver*> inputDrivers = {};
    // inputDrivers.push_back(kbd::loadPS2Driver());
    dbg::popTrace();
    return inputDrivers;
}
}; // namespace drivers::input