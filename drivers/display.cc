#include "font.h"

#include <algorithm>
#include <cassert>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <cstring>
#include <drivers/display.h>
#define MODULE "Display Driver"
extern drivers::DisplayDriver* displayDriver;

namespace drivers {
static limine_framebuffer_request
    __attribute__((used, retain, section(".limine_requests"))) fbRequest = {
        .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = nullptr};
DisplayDriver::DisplayDriver() : Driver(driver::driverType::DISPLAY) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (fbRequest.response == nullptr) {
        dbg::printm(MODULE, "ERROR: Limine failed to set framebuffer response\n");
        std::abort();
    }
    this->screenX = MARGIN;
    this->screenY = MARGIN;
    this->infos.reserve(fbRequest.response->framebuffer_count);
    for (uint64_t i = 0; i < fbRequest.response->framebuffer_count; ++i) {
        dbg::printm(
            MODULE, "Loaded display %lux%lux%lu\n", fbRequest.response->framebuffers[i]->width,
            fbRequest.response->framebuffers[i]->height, fbRequest.response->framebuffers[i]->bpp);
        this->infos.push_back(fbRequest.response->framebuffers[i]);
    }
    this->buffer = new uint8_t*[this->infos.size()];
    for (size_t i = 0; i < this->infos.size(); ++i) {
        this->buffer[i] = (uint8_t*)this->infos.at(i)->address;
    }
    this->setDriverName("Display driver");
    dbg::popTrace();
}
DisplayDriver::~DisplayDriver() {}
size_t DisplayDriver::getScreenX() {
    return this->screenX;
}
size_t DisplayDriver::getScreenY() {
    return this->screenY;
}
void DisplayDriver::setScreenX(size_t x) {
    this->screenX = x;
}
void DisplayDriver::setScreenY(size_t y) {
    this->screenY = y;
}
void DisplayDriver::init(pci::device* dev) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "TODO: Initialize display driver (%llx:%llx)\n", dev->vendorID,
                dev->deviceID);
    dbg::popTrace();
    return;
}
void DisplayDriver::deinit() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "TODO: Deinitialize display driver\n");
    dbg::popTrace();
    return;
}

void DisplayDriver::drawCharacter(uint8_t display, char c) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    for (uint8_t y = 0; y < CHAR_HEIGHT + MARGIN; ++y) {
        for (uint8_t x = 0; x < CHAR_WIDTH + MARGIN; ++x) {
            this->writePixel(display, this->screenX + x, this->screenY + y, 0x00000000);
        }
    }
    for (uint8_t y = 0; y < CHAR_HEIGHT; ++y) {
        for (uint8_t x = 0; x < CHAR_WIDTH; ++x) {
            if (CHECK_PIXEL(c, x, y)) {
                this->writePixel(display, this->screenX + x, this->screenY + y, 0xFFFFFFFF);
            }
        }
    }
    this->screenX += CHAR_WIDTH + MARGIN;
    dbg::popTrace();
}
void DisplayDriver::drawChar(uint8_t displayIdx, char c) {
    limine_framebuffer* display = this->infos.at(displayIdx);
    switch (c) {
    case '\n': {
        this->screenY += CHAR_HEIGHT + MARGIN;
        this->screenX = MARGIN;
    } break;
    case '\t': {
        this->screenX += 4 * (CHAR_WIDTH + MARGIN);
    } break;
    case '\b': {
        if (this->screenX >= CHAR_WIDTH + MARGIN) {
            this->screenX -= CHAR_WIDTH + MARGIN;
        } else {
            if (this->screenY >= CHAR_HEIGHT + MARGIN) {
                this->screenY -= CHAR_HEIGHT + MARGIN;
                bool           found        = false;
                const uint64_t line_start_y = this->screenY;
                const uint64_t line_end_y   = line_start_y + CHAR_HEIGHT + MARGIN;
                for (int64_t cell_x =
                         ((display->width - 1) / (CHAR_WIDTH + MARGIN)) * (CHAR_WIDTH + MARGIN);
                     cell_x >= 0 && !found; cell_x -= CHAR_WIDTH + MARGIN) {
                    for (uint64_t y = line_start_y; y < line_end_y && !found; y++) {
                        for (uint64_t x_offset = 0; x_offset < CHAR_WIDTH + MARGIN && !found;
                             x_offset++) {
                            uint64_t x = cell_x + x_offset;
                            if (x >= display->width) continue;
                            uint8_t r, g, b, a;
                            this->readPixel(displayIdx, x, y, &r, &g, &b, &a);
                            if (r != 0 || g != 0 || b != 0) {

                                this->screenX = cell_x + CHAR_WIDTH + (MARGIN * 2);
                                found         = true;
                            }
                        }
                    }
                }
                if (!found) {
                    this->screenX = MARGIN;
                }
            } else {
                break;
            }
        }
        this->screenX =
            std::clamp(this->screenX, (uint64_t)MARGIN, display->width - CHAR_WIDTH + MARGIN);
        this->screenY =
            std::clamp(this->screenY, (uint64_t)MARGIN, display->height - CHAR_HEIGHT + MARGIN);

        for (uint8_t y = 0; y < CHAR_HEIGHT + MARGIN; ++y) {
            for (uint8_t x = 0; x < CHAR_WIDTH + MARGIN; ++x) {
                this->writePixel(displayIdx, this->screenX + x, this->screenY + y, 0x00000000);
            }
        }
    } break;
    default:
        this->drawCharacter(displayIdx, c);
    }
    if (this->screenX + CHAR_WIDTH + MARGIN > display->width) {
        this->screenX = MARGIN;
        this->screenY += CHAR_HEIGHT + MARGIN;
    }
    while (this->screenY + CHAR_HEIGHT + MARGIN > display->height) {
        this->scrollBack(displayIdx, CHAR_HEIGHT + MARGIN);
        this->screenY -= CHAR_HEIGHT + MARGIN;
    }
}
void DisplayDriver::scrollBack(uint8_t displayIdx, uint64_t lines) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    limine_framebuffer* display = this->infos.at(displayIdx);
    if (!display || lines == 0 || display->height == 0) {
        dbg::popTrace();
        return;
    }
    if (lines >= display->height) {
        std::memset(this->buffer[displayIdx], 0, display->pitch * display->height);
        dbg::popTrace();
        return;
    }
    uint8_t* fb     = static_cast<uint8_t*>(this->buffer[displayIdx]);
    uint32_t pitch  = display->pitch;
    uint64_t height = display->height;
    for (uint64_t y = 0; y < height - lines; ++y) {
        std::memmove(fb + y * pitch, fb + (y + lines) * pitch, pitch);
    }
    const uint64_t remaining = height - lines;
    std::memset(fb + remaining * pitch, 0, lines * pitch);
    dbg::popTrace();
}
void DisplayDriver::writePixel(uint8_t display, uint64_t x, uint64_t y, uint32_t rgba) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    this->writePixel(display, x, y, (rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF,
                     rgba & 0xFF);
    dbg::popTrace();
}
void DisplayDriver::writePixel(uint8_t displayIdx, uint64_t x, uint64_t y, uint8_t r, uint8_t g,
                               uint8_t b, uint8_t a) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    limine_framebuffer* display = this->infos.at(displayIdx);
    if (x > display->width || y > display->height) {
        DisplayDriver* temp = displayDriver;
        displayDriver       = nullptr;
        dbg::printf("X %llu or Y %llu pos oob", x, y);
        displayDriver = temp;
        dbg::popTrace();
        return;
    }
    uint64_t offset    = (y * display->width + x) * (display->bpp / 8);
    auto     pixelAddr = reinterpret_cast<uint8_t*>(this->buffer[displayIdx]) + offset;
    uint8_t  newR      = (r * a) / 255;
    uint8_t  newG      = (g * a) / 255;
    uint8_t  newB      = (b * a) / 255;
    uint8_t  rShift    = display->red_mask_shift;
    uint8_t  gShift    = display->green_mask_shift;
    uint8_t  bShift    = display->blue_mask_shift;
    uint32_t rMask     = (1 << display->red_mask_size) - 1;
    uint32_t gMask     = (1 << display->green_mask_size) - 1;
    uint32_t bMask     = (1 << display->blue_mask_size) - 1;
    *reinterpret_cast<uint32_t*>(pixelAddr) &= ~(rMask << rShift);
    *reinterpret_cast<uint32_t*>(pixelAddr) &= ~(gMask << gShift);
    *reinterpret_cast<uint32_t*>(pixelAddr) &= ~(bMask << bShift);
    *reinterpret_cast<uint32_t*>(pixelAddr) |= (newR & rMask) << rShift;
    *reinterpret_cast<uint32_t*>(pixelAddr) |= (newG & gMask) << gShift;
    *reinterpret_cast<uint32_t*>(pixelAddr) |= (newB & bMask) << bShift;
    dbg::popTrace();
}
void DisplayDriver::readPixel(uint8_t display, uint64_t x, uint64_t y, uint32_t* rgba) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    uint8_t r, g, b, a;
    this->readPixel(display, x, y, &r, &g, &b, &a);
    *(uint8_t*)((uint8_t*)rgba + 0) = r;
    *(uint8_t*)((uint8_t*)rgba + 1) = g;
    *(uint8_t*)((uint8_t*)rgba + 2) = b;
    *(uint8_t*)((uint8_t*)rgba + 3) = a;
    dbg::popTrace();
}
void DisplayDriver::readPixel(uint8_t displayIdx, uint64_t x, uint64_t y, uint8_t* r, uint8_t* g,
                              uint8_t* b, uint8_t* a) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    limine_framebuffer* display = this->infos.at(displayIdx);
    if (x > display->width || y > display->height) {
        *r                  = 0;
        *g                  = 0;
        *b                  = 0;
        *a                  = 0;
        DisplayDriver* temp = displayDriver;
        displayDriver       = nullptr;
        dbg::printf("X %llu or Y %llu pos oob", x, y);
        displayDriver = temp;
        dbg::popTrace();
        return;
    }
    uint64_t offset       = (y * display->width + x) * (display->bpp / 8);
    uint8_t* pixelAddress = (uint8_t*)(this->buffer[displayIdx]) + offset;
    uint32_t pixelValue   = 0;
    for (uint64_t i = 0; i < display->bpp / 8; ++i) {
        pixelValue |= pixelAddress[i] << (i * 8);
    }
    uint8_t  rShift = display->red_mask_shift;
    uint8_t  gShift = display->green_mask_shift;
    uint8_t  bShift = display->blue_mask_shift;
    uint32_t rMask  = (1 << display->red_mask_size) - 1;
    uint32_t gMask  = (1 << display->green_mask_size) - 1;
    uint32_t bMask  = (1 << display->blue_mask_size) - 1;
    *r              = (pixelValue >> rShift) & rMask;
    *g              = (pixelValue >> gShift) & gMask;
    *b              = (pixelValue >> bShift) & bMask;
    *a              = 255;
    dbg::popTrace();
}
DisplayDriver* loadDisplayDriver(pci::device* device) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    DisplayDriver* driver = new DisplayDriver();
    driver->init(device);
    dbg::popTrace();
    return driver;
}
}; // namespace drivers