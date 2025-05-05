#if !defined(_KERNEL_DRIVER_DRIVER_H_)
#define _KERNEL_DRIVER_DRIVER_H_
#include <cstddef>
#include <vector>
#include <kernel/hal/pci/pci.h>

namespace driver{
    enum struct driverType : int {
        BLOCK,
        FILESYSTEM,
        DISPLAY,
    };
    class Driver{
        public:
            Driver(driverType type);
            virtual ~Driver();
            virtual void init(pci::device* dev) = 0;
            virtual void deinit() = 0;
            void setDriverName(const char* name);
            driverType getDeviceType();
        private:
            driverType __driver_type;
            const char* __driver_name;
    };
    void initPS2Keyboard();
    void initialize();
    bool isInitialized();
    size_t getDevicesCount(driverType type);
    std::vector<Driver*> getDrivers(driverType type);
    void printInfo();
};


#endif // _KERNEL_DRIVER_DRIVER_H_
