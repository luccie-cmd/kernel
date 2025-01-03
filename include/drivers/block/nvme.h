#if !defined(_DRIVERS_BLOCK_NVME_H_)
#define _DRIVERS_BLOCK_NVME_H_
#include <kernel/driver/msc.h>

namespace drivers::block{
    class NVMeDriver : public driver::MSCDriver{};
    NVMeDriver* loadNVMeDriver(pci::device* device);
};

#endif // _DRIVERS_BLOCK_NVME_H_
