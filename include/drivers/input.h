#if !defined(_DRIVERS_INPUT_H_)
#define _DRIVERS_INPUT_H_
#include <kernel/driver/driver.h>
#include <vector>

namespace drivers::input
{
enum struct InputType
{
    Keyboard,
};
class InputDriver : public driver::Driver
{
  public:
    InputDriver(InputType inputType);
    virtual ~InputDriver() = 0;
    virtual void init(pci::device* dev) = 0;
    virtual void deinit() = 0;

  protected:
    InputType __inputType;
};
std::vector<InputDriver*> loadInputDrivers();
}; // namespace drivers

#endif // _DRIVERS_INPUT_H_
