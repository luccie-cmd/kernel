#if !defined(_KERNEL_DRIVER_DISPLAY_H_)
#define _KERNEL_DRIVER_DISPLAY_H_
#include <kernel/driver/driver.h>
#include <limine/limine.h>
#include <mutex>
#include <common/spinlock.h>
#include <utility>

namespace drivers
{
    class DisplayDriver : public driver::Driver
    {
    public:
        DisplayDriver();
        ~DisplayDriver();
        void init(pci::device *dev);
        void deinit();
        void drawChar(uint8_t display, char c);
        size_t getScreenX();
        size_t getScreenY();
        void setScreenX(size_t x);
        void setScreenY(size_t y);
    private:
        void drawCharacter(uint8_t display, char c);
        void scrollBack(uint8_t display, uint64_t lines);
        void readPixel(uint8_t display, uint64_t x, uint64_t y, uint32_t *rgba);
        void readPixel(uint8_t display, uint64_t x, uint64_t y, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a);
        void writePixel(uint8_t display, uint64_t x, uint64_t y, uint32_t rgba);
        void writePixel(uint8_t display, uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        std::vector<std::pair<std::Spinlock*, limine_framebuffer *>> infos;
        std::size_t screenX, screenY;
    };
    DisplayDriver *loadDisplayDriver(pci::device *dev);
};

#endif // _KERNEL_DRIVER_DISPLAY_H_
