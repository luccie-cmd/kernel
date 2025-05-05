#include <drivers/input/kbd.h>

namespace drivers::input::kbd
{
KeyboardDriver::KeyboardDriver(KeyboardType type) : InputDriver(InputType::Keyboard)
{
    this->__kbdType = type;
}
KeyboardDriver::~KeyboardDriver() {}
KeyboardDriver* loadKbdDriver()
{
    return nullptr;
}
}; // namespace drivers::input::kbd