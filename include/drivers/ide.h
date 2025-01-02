/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_DRIVERS_IDE_H_)
#define _DRIVERS_IDE_H_
#include <kernel/driver/msc.h>

namespace drivers{
    struct IDEDevice {
       uint8_t Reserved;    // 0 (Empty) or 1 (This Drive really exists).
       uint8_t Channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
       uint8_t Drive;       // 0 (Master Drive) or 1 (Slave Drive).
       uint16_t Type;        // 0: ATA, 1:ATAPI.
       uint16_t Signature;   // Drive Signature
       uint16_t Capabilities;// Features.
       uint32_t CommandSets; // Command Sets Supported.
       uint32_t Size;        // Size in Sectors.
       uint8_t Model[41];   // Model in string.
    };
    struct IDEChannelRegisters {
       uint16_t base;  // I/O Base.
       uint16_t ctrl;  // Control Base
       uint16_t bmide; // Bus Master IDE
       uint8_t nIEN;  // nIEN (No Interrupt);
    };
    class IDEDriver : public driver::MSCDriver{
        public:
            IDEDriver();
            ~IDEDriver();
            bool read(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer);
            bool write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer);
            void init(pci::device* device);
            void deinit();
            uint8_t getConnectedDrives();
            uint64_t getDiskSize(uint8_t disk);
        private:
            uint8_t drives;
            IDEDevice devices[4];
            IDEChannelRegisters channels[2];
            uint8_t readReg(uint8_t channel, uint8_t reg);
            void writeReg(uint8_t channel, uint8_t reg, uint8_t data);
            void readBuffer(uint8_t channel, uint8_t reg, void* buffer, uint32_t quads);
            int poll(uint8_t channel, bool advanced);
    };
    IDEDriver* loadIDEController(pci::device* device);
};

#endif // _DRIVERS_IDE_H_
