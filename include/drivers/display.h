#if !defined(_KERNEL_DRIVER_DISPLAY_H_)
#define _KERNEL_DRIVER_DISPLAY_H_
#include <kernel/driver/driver.h>

namespace drivers{
    enum struct DisplayType : int{
        Vga,
        Xga,
        D3Controller,
        Other,
    };
    class DisplayDriver : public driver::Driver{
        public:
            DisplayDriver(DisplayType type);
            virtual ~DisplayDriver() = 0;
            virtual void init(pci::device* dev) = 0;
            virtual void deinit() = 0;
            virtual void writePixel(uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
            virtual void writePixel(uint64_t x, uint64_t y, uint32_t rgba) = 0;
        private:
            DisplayType __displayType;
        protected:
            uint16_t width, height;
            uint64_t baseAddr;
    };
    DisplayDriver* loadDisplayDriver(pci::device* dev);
};

#endif // _KERNEL_DRIVER_DISPLAY_H_
