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
    static uint32_t readConfig(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset){
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (function << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
        io::outl(0xCF8, address);
        uint32_t tmp = io::inl(0xCFC);
        dbg::popTrace();
        return tmp;
    }
    static uint16_t readConfigWord(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset){
        return (uint16_t)((readConfig(bus, slot, function, offset) >> ((offset & 2) * 8)) & 0xFFFF);
    }
    static void writeConfig(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t data){
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint32_t address = (uint32_t)((bus << 16) | (slot << 11) | (function << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
        io::outl(0xCF8, address);
        io::outl(0xCFC, data);
        dbg::popTrace();
    }
    void writeConfig(device* dev, uint8_t offset, uint32_t data){
        writeConfig(dev->bus, dev->slot, dev->function, offset, data);
    }
    uint32_t readConfig(device* dev, uint8_t offset){
        return readConfig(dev->bus, dev->slot, dev->function, offset);
    }
    uint16_t readConfigWord(device* dev, uint8_t offset){
        return readConfigWord(dev->bus, dev->slot, dev->function, offset);
    }
    void enableBusmaster(device* dev){
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint16_t cmd = readConfig(dev, 0x04);
        uint16_t status = readConfig(dev, 0x06);
        cmd |= (1 << 2);
        writeConfig(dev, 0x04, (uint32_t)status << 16 | (uint32_t) cmd);
        dbg::popTrace();
    }
    static uint16_t getVendor(uint8_t bus, uint8_t slot, uint8_t func){
        return readConfigWord(bus, slot, func, 0);
    }
    static uint16_t getDevice(uint8_t bus, uint8_t slot, uint8_t func){
        return readConfigWord(bus, slot, func, 2);
    }
    static uint8_t getClassCode(uint8_t bus, uint8_t slot, uint8_t function){
        return (uint8_t)((readConfigWord(bus, slot, function, 0xA) & ~0x00FF) >> 8);
    }
    static uint8_t getSubClassCode(uint32_t bus, uint32_t slot, uint32_t function){
        return (uint8_t)((readConfigWord(bus, slot, function, 0xA) & ~0xFF00));
    }
    static void loopBus(uint8_t endBus){
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
    }
    void initialize(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        devices.clear();
        loopBus(0xff);
        initialized = true;
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