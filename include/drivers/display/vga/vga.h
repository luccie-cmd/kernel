#if !defined(_DRIVERS_DISPLAY_VGA_VGA_H_)
#define _DRIVERS_DISPLAY_VGA_VGA_H_
#include <drivers/display.h>

namespace drivers::display{
    class VgaDriver : public DisplayDriver{
        public:
            VgaDriver();
            ~VgaDriver();
            void init(pci::device* dev);
            void deinit();
            void writePixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
            void writePixel(uint64_t x, uint64_t y, uint32_t rgba); 
    };
    VgaDriver* loadVgaDriver(pci::device* device);
};

#endif // _DRIVERS_DISPLAY_VGA_VGA_H_
