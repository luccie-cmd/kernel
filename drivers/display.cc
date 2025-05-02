#include "font.h"
#include <cassert>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <drivers/display.h>
#define MODULE "Display Driver"
extern drivers::DisplayDriver *displayDriver;

namespace drivers
{
    limine_framebuffer_request fbRequest = {
        .id = LIMINE_FRAMEBUFFER_REQUEST,
        .revision = 0,
        .response = nullptr};
    DisplayDriver::DisplayDriver() : Driver(driver::driverType::DISPLAY)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        if (fbRequest.response == nullptr)
        {
            dbg::printm(MODULE, "ERROR: Limine failed to set framebuffer response\n");
            std::abort();
        }
        this->screenX = 2;
        this->screenY = 2;
        this->infos.reserve(fbRequest.response->framebuffer_count);
        for (uint64_t i = 0; i < fbRequest.response->framebuffer_count; ++i)
        {
            this->infos.push_back(fbRequest.response->framebuffers[i]);
        }
        this->setDriverName("Display driver");
        dbg::popTrace();
    }
    DisplayDriver::~DisplayDriver() {}
    void DisplayDriver::init(pci::device *dev)
    {
        (void)dev;
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Display driver shouldn't be initialized with PCI device!!!\n");
        for (;;)
            ;
    }
    void DisplayDriver::deinit()
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Display driver shouldn't be deinitialized with PCI device!!!\n");
        for (;;)
            ;
    }

    void DisplayDriver::drawCharacter(uint8_t display, char c)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        limine_framebuffer *buffer = this->infos.at(display);
        for (int y = 0; y < 8; ++y)
        {
            for (int x = 0; x < 8; ++x)
            {
                if (FONT[(uint8_t)c][y] & (1 << x))
                {
                    this->writePixel(display, this->screenX + x, this->screenY + y, 0xFFFFFFFF);
                }
            }
        }
        this->screenX += (10);
        if (this->screenX >= buffer->width)
        {
            this->screenX = 0;
            this->screenY += 10;
        }
        if (this->screenY >= buffer->height)
        {
            this->scrollBack(display, 10);
            this->screenY -= 10;
        }
        dbg::popTrace();
    }
    void DisplayDriver::drawChar(uint8_t display, char c)
    {
        switch (c)
        {
        case '\n':
            this->screenY += 10;
            this->screenX = 2;
            break;
        case '\t':
            this->screenX += 40;
            break;
        default:
            this->drawCharacter(display, c);
        }
    }
    void DisplayDriver::scrollBack(uint8_t display, uint64_t lines)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        limine_framebuffer *buffer = this->infos.at(display);
        for (uint64_t y = lines; y < buffer->height; ++y)
        {
            for (uint64_t x = 0; x < buffer->width; ++x)
            {
                uint8_t r, g, b, a;
                this->readPixel(display, x, y, &r, &g, &b, &a);
                this->writePixel(display, x, y - lines, r, g, b, a);
            }
        }
        for (uint64_t y = buffer->height - lines; y < buffer->height; ++y)
        {
            for (uint64_t x = 0; x < buffer->width; ++x)
            {
                this->writePixel(display, x, y, 0, 0, 0, 0);
            }
        }
        dbg::popTrace();
    }
    void DisplayDriver::writePixel(uint8_t display, uint64_t x, uint64_t y, uint32_t rgba)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->writePixel(display, x, y, (rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF, rgba & 0xFF);
        dbg::popTrace();
    }
    void DisplayDriver::writePixel(uint8_t display, uint64_t x, uint64_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        limine_framebuffer *buffer = this->infos.at(display);
        if (x > buffer->width || y > buffer->height)
        {
            displayDriver = nullptr;
            dbg::print("X or Y pos oob");
            dbg::popTrace();
            return;
        }
        uint64_t offset = (y * buffer->width + x) * (buffer->bpp / 8);
        auto pixelAddr = reinterpret_cast<volatile uint8_t *>(buffer->address) + offset;
        uint8_t newR = (r * a) / 255;
        uint8_t newG = (g * a) / 255;
        uint8_t newB = (b * a) / 255;
        uint8_t rShift = buffer->red_mask_shift;
        uint8_t gShift = buffer->green_mask_shift;
        uint8_t bShift = buffer->blue_mask_shift;
        uint32_t rMask = (1 << buffer->red_mask_size) - 1;
        uint32_t gMask = (1 << buffer->green_mask_size) - 1;
        uint32_t bMask = (1 << buffer->blue_mask_size) - 1;
        *reinterpret_cast<volatile uint32_t *>(pixelAddr) &= ~(rMask << rShift);
        *reinterpret_cast<volatile uint32_t *>(pixelAddr) &= ~(gMask << gShift);
        *reinterpret_cast<volatile uint32_t *>(pixelAddr) &= ~(bMask << bShift);
        *reinterpret_cast<volatile uint32_t *>(pixelAddr) |= (newR & rMask) << rShift;
        *reinterpret_cast<volatile uint32_t *>(pixelAddr) |= (newG & gMask) << gShift;
        *reinterpret_cast<volatile uint32_t *>(pixelAddr) |= (newB & bMask) << bShift;
        dbg::popTrace();
    }
    void DisplayDriver::readPixel(uint8_t display, uint64_t x, uint64_t y, uint32_t *rgba)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint8_t r, g, b, a;
        this->readPixel(display, x, y, &r, &g, &b, &a);
        *(uint8_t *)(rgba + 0) = r;
        *(uint8_t *)(rgba + 1) = g;
        *(uint8_t *)(rgba + 2) = b;
        *(uint8_t *)(rgba + 3) = a;
        dbg::popTrace();
    }
    void DisplayDriver::readPixel(uint8_t display, uint64_t x, uint64_t y, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a)
    {
        dbg::addTrace(__PRETTY_FUNCTION__);
        limine_framebuffer *buffer = this->infos.at(display);
        uint64_t offset = (y * buffer->width + x) * (buffer->bpp / 8);
        volatile uint8_t *pixelAddress = (volatile uint8_t *)(buffer->address) + offset;
        uint32_t pixelValue = 0;
        for (uint64_t i = 0; i < buffer->bpp / 8; ++i)
        {
            pixelValue |= pixelAddress[i] << (i * 8);
        }
        uint8_t rShift = buffer->red_mask_shift;
        uint8_t gShift = buffer->green_mask_shift;
        uint8_t bShift = buffer->blue_mask_shift;
        uint32_t rMask = (1 << buffer->red_mask_size) - 1;
        uint32_t gMask = (1 << buffer->green_mask_size) - 1;
        uint32_t bMask = (1 << buffer->blue_mask_size) - 1;
        *r = (pixelValue >> rShift) & rMask;
        *g = (pixelValue >> gShift) & gMask;
        *b = (pixelValue >> bShift) & bMask;
        *a = 255;
        dbg::popTrace();
    }
    DisplayDriver *loadDisplayDriver(pci::device *device)
    {
        (void)device;
        dbg::addTrace(__PRETTY_FUNCTION__);
        DisplayDriver *driver = new DisplayDriver();
        dbg::popTrace();
        return driver;
    }
};