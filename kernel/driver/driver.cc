#include <algorithm>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/ordered_map.h>
#include <cstdlib>
#include <drivers/display.h>
#include <drivers/msc.h>
#include <kernel/driver/driver.h>
#include <vector>
#define MODULE "Driver manager"

namespace driver
{
driverType Driver::getDeviceType()
{
    return this->__driver_type;
}
static bool                 initialized = false;
static std::vector<Driver*> drivers;
static Driver*              loadDriver(pci::device* device)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    Driver* driver = nullptr;
    switch (device->classCode)
    {
    case 0x1:
    {
        driver = drivers::loadMSCdriver(device);
    }
    break;
    case 0x2:
    {
        dbg::printm(MODULE, "TODO: Setup networking (subclass 0x%02x progIF 0x%02x)\n",
                    device->subclassCode, (uint8_t)(pci::readConfigWord(device, 0x08) >> 8));
    }
    break;
    case 0x3:
    {
        driver = drivers::loadDisplayDriver(device);
    }
    break;
    case 0x4:
    {
        dbg::printm(MODULE, "TODO: Setup multimedia controller (Subclass 0x%02x)\n",
                    device->subclassCode);
    }
    break;
    case 0xc:
    {
        dbg::printm(MODULE, "TODO: Load Serial Bus controllers (subclass 0x%02x progIF 0x%02x)\n",
                    device->subclassCode, (uint8_t)(pci::readConfigWord(device, 0x08) >> 8));
    }
    break;
    case 0x6:
    {
        dbg::printm(MODULE, "TODO: Load bridge drivers (subclass 0x%02x progIF 0x%02x)\n",
                    device->subclassCode, (uint8_t)(pci::readConfigWord(device, 0x08) >> 8));
    }
    break;
    default:
    {
        dbg::printm(MODULE, "TODO: Load driver for class %02x\n", device->classCode);
        std::abort();
    }
    break;
    }
    dbg::popTrace();
    return driver;
}
void initialize()
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    drivers.clear();
    std::vector<pci::device*> pciDevices = pci::getAllDevices();
    for (pci::device* device : pciDevices)
    {
        Driver* driver = loadDriver(device);
        if (driver == nullptr)
        {
            continue;
        }
        drivers.push_back(driver);
    }
    initialized = true;
    dbg::popTrace();
}
bool isInitialized()
{
    return initialized;
}
size_t getDevicesCount(driverType type)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized())
    {
        initialize();
    }
    size_t count = 0;
    for (Driver* driver : drivers)
    {
        if (driver->getDeviceType() == type)
        {
            count++;
        }
    }
    dbg::popTrace();
    return count;
}
std::vector<Driver*> getDrivers(driverType type)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized())
    {
        initialize();
    }
    if (getDevicesCount(type) == 0)
    {
        dbg::printm(MODULE,
                    "Unable to search for device type %d if there are no devices like that\n",
                    (int)type);
        std::abort();
    }
    std::vector<Driver*> retDrivers;
    for (Driver* driver : drivers)
    {
        if (driver->getDeviceType() == type)
        {
            retDrivers.push_back(driver);
        }
    }
    dbg::popTrace();
    return retDrivers;
}
Driver::Driver(driverType type) : __driver_type(type) {}
Driver::~Driver() {}
void Driver::setDriverName(const char* name)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    this->__driver_name = name;
    dbg::popTrace();
}
void printInfo()
{
    dbg::printm(MODULE, "TODO: Print info\n");
}
// TODO: Keypad specific stuff. For now we just map them to their numbers and things
static OrderedMap<uint8_t, char> normalScancodes = {
    {0x0D, '\t'},
    {0x0E, '`'},
    {0x15, 'q'},
    {0x16, '1'},
    {0x1A, 'z'},
    {0x1B, 's'},
    {0x1C, 'a'},
    {0x1D, 'w'},
    {0x1E, '2'},
    {0x21, 'c'},
    {0x22, 'x'},
    {0x23, 'd'},
    {0x24, 'e'},
    {0x25, '4'},
    {0x26, '3'},
    {0x29, ' '},
    {0x2A, 'v'},
    {0x2B, 'f'},
    {0x2C, 't'},
    {0x2D, 'r'},
    {0x2E, '5'},
    {0x31, 'n'},
    {0x32, 'b'},
    {0x33, 'h'},
    {0x34, 'g'},
    {0x35, 'y'},
    {0x36, '6'},
    {0x3A, 'm'},
    {0x3B, 'j'},
    {0x3C, 'u'},
    {0x3D, '7'},
    {0x3E, '8'},
    {0x41, ','},
    {0x42, 'k'},
    {0x43, 'i'},
    {0x44, 'o'},
    {0x45, '0'},
    {0x46, '9'},
    {0x49, '.'},
    {0x4A, '/'},
    {0x4B, 'l'},
    {0x4C, ';'},
    {0x4D, 'p'},
    {0x4E, '-'},
    {0x52, '\''},
    {0x54, '['},
    {0x55, '='},
    {0x5A, '\n'},
    {0x5B, ']'},
    {0x5D, '\\'},
    {0x66, '\b'},
    // Keypad
    {0x69, '1'},
    {0x6B, '4'},
    {0x6C, '7'},
    {0x70, '0'},
    {0x71, '.'},
    {0x72, '2'},
    {0x73, '5'},
    {0x74, '6'},
    {0x75, '8'},
    {0x76, '\e'},
    {0x79, '+'},
    {0x7A, '3'},
    {0x7B, '-'},
    {0x7C, '*'},
    {0x7D, '9'},
    // End keypad
};
static OrderedMap<uint8_t, char> shiftScancodes = {
    {0x0D, '\t'},
    {0x0E, '~'},
    {0x15, 'Q'},
    {0x16, '!'},
    {0x1A, 'Z'},
    {0x1B, 'S'},
    {0x1C, 'A'},
    {0x1D, 'W'},
    {0x1E, '@'},
    {0x21, 'C'},
    {0x22, 'X'},
    {0x23, 'D'},
    {0x24, 'E'},
    {0x25, '$'},
    {0x26, '#'},
    {0x29, ' '},
    {0x2A, 'V'},
    {0x2B, 'F'},
    {0x2C, 'T'},
    {0x2D, 'R'},
    {0x2E, '%'},
    {0x31, 'N'},
    {0x32, 'B'},
    {0x33, 'H'},
    {0x34, 'G'},
    {0x35, 'Y'},
    {0x36, '^'},
    {0x3A, 'M'},
    {0x3B, 'J'},
    {0x3C, 'U'},
    {0x3D, '&'},
    {0x3E, '*'},
    {0x41, '<'},
    {0x42, 'K'},
    {0x43, 'I'},
    {0x44, 'O'},
    {0x45, ')'},
    {0x46, '('},
    {0x49, '>'},
    {0x4A, '?'},
    {0x4B, 'L'},
    {0x4C, ':'},
    {0x4D, 'P'},
    {0x4E, '_'},
    {0x52, '\"'},
    {0x54, '{'},
    {0x55, '+'},
    {0x5A, '\n'},
    {0x5B, '}'},
    {0x5D, '|'},
    {0x66, '\b'},
    // Keypad
    {0x69, '1'},
    {0x6B, '4'},
    {0x6C, '7'},
    {0x70, '0'},
    {0x71, '.'},
    {0x72, '2'},
    {0x73, '5'},
    {0x74, '6'},
    {0x75, '8'},
    {0x76, '\e'},
    {0x79, '+'},
    {0x7A, '3'},
    {0x7B, '-'},
    {0x7C, '*'},
    {0x7D, '9'},
    // End keypad
};
static char translateScancode(uint8_t scancode, bool pressed, bool special)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    static bool shift    = false;
    static bool capsLock = false;
    switch (scancode)
    {
    case 0x59: // Right Shift
    case 0x12: // Left Shift
    {
        shift = capsLock ? !pressed : pressed;
        dbg::popTrace();
        return '\0';
    }
    case 0x58: // Capslock
    {
        capsLock = pressed ? !capsLock : capsLock;
        shift    = capsLock;
        dbg::popTrace();
        return '\0';
    }
    break;
    }
    if (!pressed)
    {
        dbg::popTrace();
        return '\0';
    }
    if (!special)
    {
        auto map = shift ? shiftScancodes : normalScancodes;
        if (!map.contains(scancode))
        {
            dbg::printm(MODULE, "TODO: Handle scancode 0x%hhx\n", scancode);
            dbg::popTrace();
            return '\0';
        }
        char asciiChar = map.at(scancode);
        dbg::popTrace();
        return asciiChar;
    }
    dbg::printm(MODULE, "TODO: Handle special scancode 0x%hhx\n", scancode);
    dbg::popTrace();
    return '\0';
}

void initPS2Keyboard()
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "Loading PS/2 Keyboard\n");
    // 0x60 = data;
    // 0x64 = status, command;
    io::outb(0x64, 0xAD);
    io::outb(0x64, 0xA7);
    (void)io::inb(0x60);
    io::outb(0x64, 0x20);
    uint8_t cc = io::inb(0x60);
    cc         = 0b00100101;
    io::outb(0x64, 0x60);
    io::outb(0x60, cc);
    io::outb(0x64, 0xAA);
    while ((io::inb(0x64) & 1) == 0)
        ;
    if (io::inb(0x60) != 0x55)
    {
        dbg::printm(MODULE, "No PS/2 keyboard present\n");
        dbg::popTrace();
        return;
    }
    io::outb(0x64, 0xAB);
    while ((io::inb(0x64) & 1) == 0)
        ;
    if (io::inb(0x60) != 0x00)
    {
        dbg::printm(MODULE, "No PS/2 keyboard present\n");
        dbg::popTrace();
        return;
    }
    io::outb(0x64, 0xAE);
    io::outb(0x60, 0xF0);
    io::outb(0x60, 0);
    while ((io::inb(0x64) & 1) == 0)
        ;
    uint8_t resp = io::inb(0x60);
    if (resp != 0xFA)
    {
        dbg::printm(MODULE, "PS/2 keyboard failed to ACK command\n");
        std::abort();
    }
    uint8_t scancodeSet = io::inb(0x60);
    dbg::printm(MODULE, "Using PS/2 scancode set 0x%hhx\n", scancodeSet);
    if (scancodeSet != 0x02)
    {
        dbg::printm(MODULE, "TODO: Support scancodeset 0x%hhx\n", scancodeSet);
        std::abort();
    }
    while (1)
    {
        while ((io::inb(0x64) & 1) == 0)
            ;
        uint8_t byte    = io::inb(0x60);
        bool    pressed = true;
        bool    normal  = true;
        if (byte == 0xE0)
        {
            byte   = io::inb(0x60);
            normal = false;
        }
        if (byte == 0xF0)
        {
            byte    = io::inb(0x60);
            pressed = false;
        }
        dbg::printf("%c", translateScancode(byte, pressed, !normal));
        // dbg::printf("%c %s%s\n", translateScancode(byte, pressed, !normal),
        // pressed ? "pressed" : "released", normal ? "" : " multimedia");
    }
    dbg::printm(MODULE, "Added PS/2 keyboard\n");
    dbg::popTrace();
}
}; // namespace driver