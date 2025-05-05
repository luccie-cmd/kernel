#if !defined(_DRIVERS_INPUT_KBD_PS2_H_)
#define _DRIVERS_INPUT_KBD_PS2_H_
#include <drivers/input/kbd.h>

namespace drivers::input::kbd
{
class PS2Driver : public KeyboardDriver
{
  public:
    PS2Driver();
    ~PS2Driver();
    void init(pci::device* dev);
    void deinit();
    std::vector<uint8_t> getKeyPresses(size_t number);
};
PS2Driver* loadPS2Driver();

}; // namespace drivers::input::kbd

#endif // _DRIVERS_INPUT_KBD_PS2_H_
