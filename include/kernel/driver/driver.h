#if !defined(_KERNEL_DRIVER_DRIVER_H_)
#define _KERNEL_DRIVER_DRIVER_H_
#include <cstddef>
#include <stl/vector>
#include <kernel/hal/pci/pci.h>

namespace driver{
    enum struct deviceType : int {
        BLOCK,
    };
    class Driver{
        public:
            Driver(deviceType type, const char* name);
            ~Driver();
            const char* getDeviceName();
            deviceType getDeviceType();
            virtual void init(pci::device* dev) = 0;
            virtual void deinit() = 0;
        private:
            deviceType __device_type;
            const char* __device_name;
    };
    void initialize();
    bool isInitialized();
    size_t getDevicesCount(deviceType type);
    std::vector<Driver*> getDrivers(deviceType type);
};


#endif // _KERNEL_DRIVER_DRIVER_H_
