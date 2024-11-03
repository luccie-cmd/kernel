#if !defined(_KERNEL_DRIVER_DRIVER_H_)
#define _KERNEL_DRIVER_DRIVER_H_
#include <cstddef>

namespace driver{
    enum struct deviceType : int {
        BLOCK,
    };
    void initialize();
    bool isInitialized();
    size_t getDevicesCount(deviceType type);
};


#endif // _KERNEL_DRIVER_DRIVER_H_
