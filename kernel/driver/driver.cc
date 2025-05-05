#include <algorithm>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/ordered_map.h>
#include <cstdlib>
#include <drivers/display.h>
#include <drivers/input.h>
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
static std::vector<Driver*> loadDriver(pci::device* device)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    std::vector<Driver*> loadDrivers = {};
    switch (device->classCode)
    {
    case 0x1:
    {
        loadDrivers = {drivers::loadMSCdriver(device)};
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
        loadDrivers = {drivers::loadDisplayDriver(device)};
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
    return loadDrivers;
}
void initialize()
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    drivers.clear();
    std::vector<drivers::input::InputDriver*> inpDrivers = drivers::input::loadInputDrivers();
    for (drivers::input::InputDriver* drv : inpDrivers)
    {
        if (drv == nullptr)
        {
            continue;
        }
        drivers.push_back(drv);
    }
    std::vector<pci::device*> pciDevices = pci::getAllDevices();
    for (pci::device* device : pciDevices)
    {
        std::vector<Driver*> loadedDrivers = loadDriver(device);
        for (Driver* driver : loadedDrivers)
        {
            if (driver == nullptr)
            {
                continue;
            }
            drivers.push_back(driver);
        }
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
}; // namespace driver