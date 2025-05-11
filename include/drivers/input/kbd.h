#if !defined(_DRIVERS_INPUT_KBD_H_)
#define _DRIVERS_INPUT_KBD_H_
#include <drivers/input.h>
#include <string>

namespace drivers::input::kbd {
enum struct KeyboardType {
    PS2,
};
enum struct TranslatedKey { None };
struct KeyboardInputPair {
    TranslatedKey data;
    bool          special;
    bool          release;
};
class KeyboardDriver : public InputDriver {
  public:
    KeyboardDriver(KeyboardType kbdType);
    virtual ~KeyboardDriver()                                           = 0;
    virtual void                           init(pci::device* dev)       = 0;
    virtual void                           deinit()                     = 0;
    virtual std::vector<KeyboardInputPair> getKeyPresses(size_t number) = 0;

  protected:
    KeyboardType __kbdType;
};
KeyboardDriver* loadKbdDriver();
}; // namespace drivers::input::kbd

#endif // _DRIVERS_INPUT_KBD_H_
