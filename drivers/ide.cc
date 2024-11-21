#include <drivers/ide.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <common/io/io.h>
#define MODULE "IDE Driver"
#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive write fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Index
#define ATA_SR_ERR     0x01    // Error

#define ATA_ER_BBK      0x80    // Bad block
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // Media changed
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // Media change request
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define ATA_PRIMARY        0x00
#define ATA_SECONDARY      0x01

#define IDE_TYPE_ATA    0x00
#define IDE_TYPE_ATAPI  0x01

namespace drivers{
    uint8_t ide_buf[2048] = {0};
    IDEDriver::IDEDriver() :MSCDriver(driver::StorageType::IDE) {
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->setDriverName("IDE Controller");
        dbg::popTrace();
    }
    IDEDriver::~IDEDriver() {}
    void IDEDriver::init(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        uint32_t BAR0 = pci::readConfig(device, 0x10);
        uint32_t BAR1 = pci::readConfig(device, 0x14);
        uint32_t BAR2 = pci::readConfig(device, 0x18);
        uint32_t BAR3 = pci::readConfig(device, 0x1C);
        uint32_t BAR4 = pci::readConfig(device, 0x20);
        this->channels[ATA_PRIMARY  ].base =  (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
        this->channels[ATA_PRIMARY  ].ctrl =  (BAR1 & 0xFFFFFFFC) + 0x1F0 * (!BAR1);
        this->channels[ATA_SECONDARY].base =  (BAR2 & 0xFFFFFFFC) + 0x1F0 * (!BAR2);
        this->channels[ATA_SECONDARY].ctrl =  (BAR3 & 0xFFFFFFFC) + 0x1F0 * (!BAR3);
        this->channels[ATA_PRIMARY  ].bmide = (BAR4 & 0xFFFFFFFC) + 0;
        this->channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8;
        this->drives = 0;
        this->writeReg(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
        this->writeReg(ATA_SECONDARY, ATA_REG_CONTROL, 2);
        for(uint8_t i = 0; i < 2; ++i){
            for(uint8_t j = 0; j < 2; ++j){
                this->devices[this->drives].Reserved = 0;
                this->writeReg(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4));
                this->writeReg(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
                if(this->readReg(i, ATA_REG_STATUS) == 0) continue;
                uint8_t err = 0;
                while (1){
                    uint8_t status = this->readReg(i, ATA_REG_STATUS);
                    if(status & ATA_SR_ERR) {
                        err = 1;
                        break;
                    }
                    if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;
                }
                uint8_t type = IDE_TYPE_ATA;
                if(err != 0){
                    uint8_t cl = this->readReg(i, ATA_REG_LBA1);
                    uint8_t ch = this->readReg(i, ATA_REG_LBA2);

                    if (cl == 0x14 && ch == 0xEB)
                       type = IDE_TYPE_ATAPI;
                    else if (cl == 0x69 && ch == 0x96)
                       type = IDE_TYPE_ATAPI;
                    else
                       continue; // Unknown Type (may not be a device).

                    this->writeReg(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
                }
                this->readBuffer(i, ATA_REG_DATA, ide_buf, 128);
                this->devices[this->drives].Reserved     = 1;
                this->devices[this->drives].Type         = type;
                this->devices[this->drives].Channel      = i;
                this->devices[this->drives].Drive        = j;
                this->devices[this->drives].Signature    = *((uint16_t *)(ide_buf + ATA_IDENT_DEVICETYPE));
                this->devices[this->drives].Capabilities = *((uint16_t *)(ide_buf + ATA_IDENT_CAPABILITIES));
                this->devices[this->drives].CommandSets  = *((uint32_t *)(ide_buf + ATA_IDENT_COMMANDSETS));
                if (this->devices[this->drives].CommandSets & (1 << 26)) {
                    this->devices[this->drives].Size = *((uint32_t *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
                } else {
                    this->devices[this->drives].Size = *((uint32_t *)(ide_buf + ATA_IDENT_MAX_LBA));
                }
                for(uint8_t k = 0; k < 40; k += 2) {
                    this->devices[this->drives].Model[k] = ide_buf[ATA_IDENT_MODEL + k + 1];
                    this->devices[this->drives].Model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
                }
                this->devices[this->drives].Model[40] = 0; // Terminate String.
                this->drives++;
            }
        }
        if(this->drives == 0){
            dbg::printm(MODULE, "No drives could be found!!!\n");
            std::abort();
        }
        for (int i = 0; i < this->drives; ++i)
        if (this->devices[i].Reserved == 1) {
            dbg::printm(MODULE, " Found %s - Size: %llx - Channel: %d Drive: %d Model - %s\n",
                  (const char *[]){"ATA", "ATAPI"}[this->devices[i].Type],
                  (this->devices[i].Size*512),
                  this->devices[i].Channel,
                  this->devices[i].Drive,
                  this->devices[i].Model);
        }
        dbg::printm(MODULE, "Loaded IDE controller with %d disks\n", this->drives);
        dbg::popTrace();
    }
    void IDEDriver::deinit(){}
    int IDEDriver::poll(uint8_t channel, bool advanced){
        for(uint8_t i = 0; i < 4; ++i){
            this->readReg(channel, ATA_REG_ALTSTATUS);
        }
        while(this->readReg(channel, ATA_REG_STATUS) & ATA_SR_BSY){}
        if(advanced){
            uint8_t status = this->readReg(channel, ATA_REG_STATUS);
            if(status & ATA_SR_ERR){
                return 2;
            }
            if(status & ATA_SR_DF){
                return 1;
            }
            if((status & ATA_SR_DRQ) == 0){
                return 3;
            }
        }
        return 0;
    }
    bool IDEDriver::read(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer){
        dbg::addTrace(__PRETTY_FUNCTION__);
        if(drive+1 > this->drives){
            dbg::printm(MODULE, "Cannot access drive %d\n", drive);
            std::abort();
        }
        uint8_t lba_mode /* 0: CHS, 1:LBA28, 2: LBA48 */, dma /* 0: No DMA, 1: DMA */, cmd;
        uint8_t lba_io[6];
        uint32_t channel = this->devices[drive].Channel; // Read the Channel.
        uint32_t slavebit = this->devices[drive].Drive; // Read the Drive [Master/Slave]
        uint32_t bus = channels[channel].base; // Bus Base, like 0x1F0 which is also data port.
        uint32_t words = 256; // Almost every ATA drive has a sector-size of 512-byte.
        uint16_t cyl, i;
        uint8_t head, sect;
        this->writeReg(channel, ATA_REG_CONTROL, channels[channel].nIEN);
        if (lba >= 0x10000000) { // Sure Drive should support LBA in this case, or you are
                            // giving a wrong LBA.
           // LBA48:
           lba_mode  = 2;
           lba_io[0] = (lba & 0x000000FF) >> 0;
           lba_io[1] = (lba & 0x0000FF00) >> 8;
           lba_io[2] = (lba & 0x00FF0000) >> 16;
           lba_io[3] = (lba & 0xFF000000) >> 24;
           lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
           lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
           head      = 0; // Lower 4-bits of HDDEVSEL are not used here.
        } else if (this->devices[drive].Capabilities & 0x200)  { // Drive supports LBA?
            // LBA28:
            lba_mode  = 1;
            lba_io[0] = (lba & 0x00000FF) >> 0;
            lba_io[1] = (lba & 0x000FF00) >> 8;
            lba_io[2] = (lba & 0x0FF0000) >> 16;
            lba_io[3] = 0; // These Registers are not used here.
            lba_io[4] = 0; // These Registers are not used here.
            lba_io[5] = 0; // These Registers are not used here.
            head      = (lba & 0xF000000) >> 24;
        } else {
            // CHS:
            lba_mode  = 0;
            sect      = (lba % 63) + 1;
            cyl       = (lba + 1  - sect) / (16 * 63);
            lba_io[0] = sect;
            lba_io[1] = (cyl >> 0) & 0xFF;
            lba_io[2] = (cyl >> 8) & 0xFF;
            lba_io[3] = 0;
            lba_io[4] = 0;
            lba_io[5] = 0;
            head      = (lba + 1  - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
        }
        dma = 0;
        while(this->readReg(channel, ATA_REG_STATUS) & ATA_SR_BSY){}
        if (lba_mode == 0)
           this->writeReg(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS.
        else
            this->writeReg(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA
        if (lba_mode == 2) {
           this->writeReg(channel, ATA_REG_SECCOUNT1,   0);
           this->writeReg(channel, ATA_REG_LBA3,   lba_io[3]);
           this->writeReg(channel, ATA_REG_LBA4,   lba_io[4]);
           this->writeReg(channel, ATA_REG_LBA5,   lba_io[5]);
        }
        this->writeReg(channel, ATA_REG_SECCOUNT0,   (uint8_t)sectors);
        this->writeReg(channel, ATA_REG_LBA0,   lba_io[0]);
        this->writeReg(channel, ATA_REG_LBA1,   lba_io[1]);
        this->writeReg(channel, ATA_REG_LBA2,   lba_io[2]);
        if (lba_mode == 0 && dma == 0) cmd = ATA_CMD_READ_PIO;
        if (lba_mode == 1 && dma == 0) cmd = ATA_CMD_READ_PIO;   
        if (lba_mode == 2 && dma == 0) cmd = ATA_CMD_READ_PIO_EXT;
        this->writeReg(channel, ATA_REG_COMMAND, cmd);               // Send the Command.
        dbg::printm(MODULE, "Reading %lld sectors from LBA %lld\n", sectors, lba);
        for (i = 0; i < sectors; i++) {
            if(this->poll(channel, true)){
                dbg::popTrace();
                return false;
            }
            __asm__ volatile (
                "rep insw\n\t"  // Receive data
                : "+D"(buffer), "+c"(words) // Output operands: buffer (RDI in 64-bit) and words are updated
                : "d"(bus)             // Input operand: bus
                : "memory"             // Clobbers
            );
            buffer = (void*)((uint8_t*)buffer + (words * 2)); // Adjust the buffer pointer
        }
        dbg::popTrace();
        return true;
    }
    bool IDEDriver::write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "TODO: implement writing of IDE controller\n");
        std::abort();
        dbg::popTrace();
    }
    IDEDriver* loadIDEController(pci::device* device){
        return new IDEDriver;
    }
    uint8_t IDEDriver::readReg(uint8_t channel, uint8_t reg){
        uint8_t result = 0;
        if (reg > 0x07 && reg < 0x0C){
            this->writeReg(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
        }
        if (reg < 0x08){
           result = io::inb(channels[channel].base+reg-0x00);
        } else if (reg < 0x0C){
           result = io::inb(channels[channel].base +reg-0x06);
        } else if (reg < 0x0E){
           result = io::inb(channels[channel].ctrl +reg-0x0A);
        } else if (reg < 0x16){
           result = io::inb(channels[channel].bmide+reg-0x0E);
        } if (reg > 0x07 && reg < 0x0C){
           this->writeReg(channel, ATA_REG_CONTROL, channels[channel].nIEN);
        }
        return result;
    }
    void IDEDriver::writeReg(uint8_t channel, uint8_t reg, uint8_t data){
        if (reg > 0x07 && reg < 0x0C){
            this->writeReg(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
        }
        if (reg < 0x08){
           io::outb(channels[channel].base+reg-0x00, data);
        } else if (reg < 0x0C){
           io::outb(channels[channel].base +reg-0x06, data);
        } else if (reg < 0x0E){
           io::outb(channels[channel].ctrl +reg-0x0A, data);
        } else if (reg < 0x16){
           io::outb(channels[channel].bmide+reg-0x0E, data);
        } if (reg > 0x07 && reg < 0x0C){
           this->writeReg(channel, ATA_REG_CONTROL, channels[channel].nIEN);
        }
    }
    void IDEDriver::readBuffer(uint8_t channel, uint8_t reg, void* buffer, uint32_t quads){
        if (reg > 0x07 && reg < 0x0C){
            this->writeReg(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
        }
        __asm__ volatile("push %rax; mov %ds, %rax; mov %rax, %es; pop %rax");
        if (reg < 0x08){
            io::insl(channels[channel].base+reg-0x00, buffer, quads);
        }
        else if (reg < 0x0C){
            io::insl(channels[channel].base+reg-0x06, buffer, quads);
        }
        else if (reg < 0x0E){
            io::insl(channels[channel].ctrl+reg-0x0A, buffer, quads);
        }
        else if (reg < 0x16){
            io::insl(channels[channel].bmide+reg-0x0E, buffer, quads);
        }
        asm volatile("mov %ds, %rax; mov %rax, %es");
        if (reg > 0x07 && reg < 0x0C){
            this->writeReg(channel, ATA_REG_CONTROL, channels[channel].nIEN);
        }
    }
};