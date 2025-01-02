/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_HAL_PCI_PCI_H_)
#define _KERNEL_HAL_PCI_PCI_H_
#include <cstdint>
#include <vector>

namespace pci{
    struct device{
        uint16_t bus;
        uint16_t slot;
        uint16_t function;
        uint16_t vendorID;
        uint16_t deviceID;
        uint8_t classCode;
        uint8_t subclassCode;
    };
    void initialize();
    bool isInitialized();
    uint16_t readConfig(device* dev, uint8_t offset);
    void enableBusmaster(device* dev);
    std::vector<device*> getAllDevices();
    void printInfo();
};

#endif // _KERNEL_HAL_PCI_PCI_H_
