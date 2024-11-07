/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/hal/pci/pci.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <common/io/io.h>
#define MODULE "PCI"

namespace pci{
    static std::vector<device*> devices;
    static bool initialized = false;
    uint16_t readConfig(uint32_t bus, uint32_t slot, uint32_t function, uint32_t offset){
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (function << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
        io::outl(0xCF8, address);
        uint16_t tmp = (uint16_t)((io::inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
        dbg::popTrace();
        return tmp;
    }
    static uint16_t getVendor(uint32_t bus, uint32_t slot, uint32_t func){
        return readConfig(bus, slot, func, 0);
    }
    static uint16_t getDevice(uint32_t bus, uint32_t slot, uint32_t func){
        return readConfig(bus, slot, func, 2);
    }
    static uint8_t getClassCode(uint32_t bus, uint32_t slot, uint32_t function){
        return (uint8_t)((readConfig(bus, slot, function, 0xA) & ~0x00FF) >> 8);
    }

    static uint8_t getSubClassCode(uint32_t bus, uint32_t slot, uint32_t function){
        return (uint8_t)((readConfig(bus, slot, function, 0xA) & ~0xFF00));
    }
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "Initializing...\n");
        devices.clear();
        for(uint32_t bus = 0; bus < 256; bus++){
            for(uint32_t slot = 0; slot < 32; slot++){
                for(uint32_t func = 0; func < 8; func++){
                    uint16_t vendorID = getVendor(bus, slot, func);
                    if(vendorID == 0xffff){
                        continue;
                    }
                    uint16_t deviceID = getDevice(bus, slot, func);
                    uint8_t classCode = getClassCode(bus, slot, func);
                    uint8_t subClassCode = getSubClassCode(bus, slot, func);
                    dbg::printm(MODULE, "Device %x:%x class %x:%x\n", vendorID, deviceID, classCode, subClassCode);
                    device* dev = new device;
                    dev->bus = bus;
                    dev->slot = slot;
                    dev->function = func;
                    dev->vendorID = vendorID;
                    dev->deviceID = deviceID;
                    dev->classCode = classCode;
                    dev->subclassCode = subClassCode;
                    devices.push_back(dev);
                }
            }
        }
        initialized = true;
        dbg::printm(MODULE, "Initialized\n");
        dbg::popTrace();
    }
    bool isInitialized(){
        return initialized;
    }
    std::vector<device*> getAllDevices(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(!isInitialized()){
            initialize();
        }
        dbg::popTrace();
        return devices;
    }
};