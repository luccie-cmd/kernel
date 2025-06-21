#include <bitset>
#include <cassert>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <drivers/block/ide.h>
#define MODULE "IDE Driver"
#define ATA_REG_DATA 0x00
#define ATA_REG_ERROR 0x01
#define ATA_REG_FEATURES 0x01
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_SECCOUNT1 0x08
#define ATA_REG_LBA3 0x09
#define ATA_REG_LBA4 0x0A
#define ATA_REG_LBA5 0x0B
#define ATA_REG_CONTROL 0x0C
#define ATA_REG_ALTSTATUS 0x0C
#define ATA_REG_DEVADDRESS 0x0D

#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

#define ATA_SR_BSY 0x80  // Busy
#define ATA_SR_DRDY 0x40 // Drive ready
#define ATA_SR_DF 0x20   // Drive write fault
#define ATA_SR_DSC 0x10  // Drive seek complete
#define ATA_SR_DRQ 0x08  // Data request ready
#define ATA_SR_CORR 0x04 // Corrected data
#define ATA_SR_IDX 0x02  // Index
#define ATA_SR_ERR 0x01  // Error

#define ATA_ER_BBK 0x80   // Bad block
#define ATA_ER_UNC 0x40   // Uncorrectable data
#define ATA_ER_MC 0x20    // Media changed
#define ATA_ER_IDNF 0x10  // ID mark not found
#define ATA_ER_MCR 0x08   // Media change request
#define ATA_ER_ABRT 0x04  // Command aborted
#define ATA_ER_TK0NF 0x02 // Track 0 not found
#define ATA_ER_AMNF 0x01  // No address mark

#define ATA_IDENT_DEVICETYPE 0
#define ATA_IDENT_CYLINDERS 2
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID 106
#define ATA_IDENT_MAX_LBA 120
#define ATA_IDENT_COMMANDSETS 164
#define ATA_IDENT_MAX_LBA_EXT 200

#define ATA_PRIMARY 0x00
#define ATA_SECONDARY 0x01

#define IDE_TYPE_ATA 0x00
#define IDE_TYPE_ATAPI 0x01

namespace drivers::block {
uint8_t ide_buf[2048] = {0};
IDEDriver::IDEDriver() : MSCDriver(StorageType::IDE) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    this->setDriverName("IDE Controller");
    dbg::popTrace();
}
IDEDriver::~IDEDriver() {}
uint8_t IDEDriver::getConnectedDrives() {
    return this->drives;
}
uint64_t IDEDriver::getDiskSize(uint8_t disk) {
    return (uint64_t)this->devices[disk].Size;
}
void IDEDriver::init(pci::device* device) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "ProgIF = %hhx\n", (uint8_t)(pci::readConfigWord(device, 0x08) >> 8));
    uint32_t BAR0                       = pci::readConfig(device, 0x10);
    uint32_t BAR1                       = pci::readConfig(device, 0x14);
    uint32_t BAR2                       = pci::readConfig(device, 0x18);
    uint32_t BAR3                       = pci::readConfig(device, 0x1C);
    uint32_t BAR4                       = pci::readConfig(device, 0x20);
    this->channels[ATA_PRIMARY].base    = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
    this->channels[ATA_PRIMARY].ctrl    = (BAR1 & 0xFFFFFFFC) + 0x1F0 * (!BAR1);
    this->channels[ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x1F0 * (!BAR2);
    this->channels[ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x1F0 * (!BAR3);
    this->channels[ATA_PRIMARY].bmide   = (BAR4 & 0xFFFFFFFC) + 0;
    this->channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8;
    uint8_t caps                        = (uint8_t)(pci::readConfigWord(device, 0x08) >> 8);
    if (caps & (1 << 7)) {
        pci::enableBusmaster(device);
    }
    this->drives = 0;
    this->writeReg(ATA_PRIMARY, ATA_REG_CONTROL, 2);
    this->writeReg(ATA_SECONDARY, ATA_REG_CONTROL, 2);
    for (uint8_t i = 0; i < 2; ++i) {
        for (uint8_t j = 0; j < 2; ++j) {
            this->devices[this->drives].Reserved = 0;
            this->writeReg(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4));
            this->writeReg(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
            if (this->readReg(i, ATA_REG_STATUS) == 0) continue;
            uint8_t  err = 0;
            uint32_t k   = 0;
            while (1) {
                uint8_t status = this->readReg(i, ATA_REG_STATUS);
                if (status & ATA_SR_ERR) {
                    err = 1;
                    break;
                }
                if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;

                k++;
                if ((k & 0xFFFFFF) == 0) {
                    break;
                }
            }
            uint8_t type = IDE_TYPE_ATA;
            if (err != 0) {
                uint8_t cl = this->readReg(i, ATA_REG_LBA1);
                uint8_t ch = this->readReg(i, ATA_REG_LBA2);

                if (cl == 0x14 && ch == 0xEB)
                    type = IDE_TYPE_ATAPI;
                else if (cl == 0x69 && ch == 0x96)
                    type = IDE_TYPE_ATAPI;
                else
                    continue;

                this->writeReg(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
            }
            this->readBuffer(i, ATA_REG_DATA, ide_buf, 128);
            this->devices[this->drives].Reserved  = 1;
            this->devices[this->drives].Type      = type;
            this->devices[this->drives].Channel   = i;
            this->devices[this->drives].Drive     = j;
            this->devices[this->drives].Signature = *((uint16_t*)(ide_buf + ATA_IDENT_DEVICETYPE));
            this->devices[this->drives].Capabilities =
                *((uint16_t*)(ide_buf + ATA_IDENT_CAPABILITIES));
            this->devices[this->drives].CommandSets =
                *((uint32_t*)(ide_buf + ATA_IDENT_COMMANDSETS));
            if (this->devices[this->drives].CommandSets & (1 << 26)) {
                this->devices[this->drives].Size = *((uint32_t*)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
            } else {
                this->devices[this->drives].Size = *((uint32_t*)(ide_buf + ATA_IDENT_MAX_LBA));
            }
            if (this->devices[this->drives].Size == 0) {
                dbg::printm(MODULE, "Got 0 size, not adding drive\n");
                continue;
            }
            for (uint8_t l = 0; l < 40; l += 2) {
                this->devices[this->drives].Model[l]     = ide_buf[ATA_IDENT_MODEL + l + 1];
                this->devices[this->drives].Model[l + 1] = ide_buf[ATA_IDENT_MODEL + l];
            }
            this->devices[this->drives].Model[40] = 0;
            this->drives++;
        }
    }
    if (this->drives == 0) {
        dbg::printm(MODULE, "Warning: No drives could be found!!!\n");
    } else {
        dbg::printm(MODULE, "Found %hd drives\n", this->drives);
    }
    dbg::popTrace();
}
void IDEDriver::deinit() {}
int  IDEDriver::poll(uint8_t channel, bool advanced) {
    for (uint8_t i = 0; i < 4; ++i) {
        this->readReg(channel, ATA_REG_ALTSTATUS);
    }
    uint32_t k = 0;
    while (this->readReg(channel, ATA_REG_STATUS) & ATA_SR_BSY) {
        k++;
        if ((k & 0xFFFFFF) == 0) {
            break;
        }
    }
    if (advanced) {
        uint8_t status = this->readReg(channel, ATA_REG_STATUS);
        if (status & ATA_SR_ERR) {
            return 2;
        }
        if (status & ATA_SR_DF) {
            return 1;
        }
        if ((status & ATA_SR_DRQ) == 0) {
            return 3;
        }
    }
    return 0;
}
bool IDEDriver::read(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (drive + 1 > this->drives) {
        dbg::printm(MODULE, "Warning: Cannot access drive %d (Max allowed: %llu)\n", drive,
                    this->drives);
        return false;
    }
    uint8_t  lba_mode, cmd;
    uint8_t  lba_io[6];
    uint32_t channel  = this->devices[drive].Channel;
    uint32_t slavebit = this->devices[drive].Drive;
    uint32_t bus      = channels[channel].base;
    uint32_t words    = SECTOR_SIZE / 2;
    uint16_t cyl, i;
    uint8_t  head, sect;
    this->writeReg(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    if (lba >= 0x10000000) {
        lba_mode  = 2;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = 0;
    } else if (this->devices[drive].Capabilities & 0x200) {
        lba_mode  = 1;
        lba_io[0] = (lba & 0x00000FF) >> 0;
        lba_io[1] = (lba & 0x000FF00) >> 8;
        lba_io[2] = (lba & 0x0FF0000) >> 16;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba & 0xF000000) >> 24;
    } else {
        dbg::printm(MODULE, "Using CHS mode instead of LBA!!!\n");
        lba_mode  = 0;
        sect      = (lba % 63) + 1;
        cyl       = (lba + 1 - sect) / (16 * 63);
        lba_io[0] = sect;
        lba_io[1] = (cyl >> 0) & 0xFF;
        lba_io[2] = (cyl >> 8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba + 1 - sect) % (16 * 63) / (63);
    }
    while (this->readReg(channel, ATA_REG_STATUS) & ATA_SR_BSY) {}
    if (lba_mode == 0)
        this->writeReg(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head);
    else
        this->writeReg(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head);
    if (lba_mode == 2) {
        this->writeReg(channel, ATA_REG_SECCOUNT1, 0);
        this->writeReg(channel, ATA_REG_LBA3, lba_io[3]);
        this->writeReg(channel, ATA_REG_LBA4, lba_io[4]);
        this->writeReg(channel, ATA_REG_LBA5, lba_io[5]);
    }
    this->writeReg(channel, ATA_REG_SECCOUNT0, (uint8_t)sectors);
    this->writeReg(channel, ATA_REG_LBA0, lba_io[0]);
    this->writeReg(channel, ATA_REG_LBA1, lba_io[1]);
    this->writeReg(channel, ATA_REG_LBA2, lba_io[2]);
    if (lba_mode == 0) cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 1) cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 2) cmd = ATA_CMD_READ_PIO_EXT;
    this->writeReg(channel, ATA_REG_COMMAND, cmd);
    for (i = 0; i < sectors; i++) {
        if (this->poll(channel, true)) {
            dbg::printm(MODULE, "Read from %hhu: %llu - %llu (%lu sectors) failed\n", drive, lba,
                        lba + sectors, sectors);
            std::abort();
        }
        __asm__ volatile("pushw %ax");
        __asm__ volatile("mov %es, %ax");
        __asm__ volatile("pushw %ax");
        __asm__ volatile("mov %%ax, %%es" : : "a"(0x10));
        __asm__ volatile("rep insw" : : "c"(words), "d"(bus), "D"(buffer));
        __asm__ volatile("popw %ax");
        __asm__ volatile("mov %ax, %es");
        __asm__ volatile("popw %ax");
        forceReadVolatile(words);
        forceReadVolatile(bus);
        forceReadVolatile(buffer);
        buffer = (void*)((uint8_t*)buffer + (words * 2));
    }
    dbg::popTrace();
    return true;
}
bool IDEDriver::write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (drive + 1 > this->drives) {
        dbg::printm(MODULE, "Warning: Cannot access drive %d (Max allowed: %llu)\n", drive,
                    this->drives);
        return false;
    }
    uint8_t  lba_mode, cmd;
    uint8_t  lba_io[6];
    uint32_t channel  = this->devices[drive].Channel;
    uint32_t slavebit = this->devices[drive].Drive;
    uint32_t bus      = channels[channel].base;
    uint32_t words    = SECTOR_SIZE / 2;
    uint16_t cyl, i;
    uint8_t  head, sect;
    this->writeReg(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    if (lba >= 0x10000000) {
        lba_mode  = 2;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = 0;
    } else if (this->devices[drive].Capabilities & 0x200) {
        lba_mode  = 1;
        lba_io[0] = (lba & 0x00000FF) >> 0;
        lba_io[1] = (lba & 0x000FF00) >> 8;
        lba_io[2] = (lba & 0x0FF0000) >> 16;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba & 0xF000000) >> 24;
    } else {
        dbg::printm(MODULE, "Using CHS mode instead of LBA!!!\n");
        lba_mode  = 0;
        sect      = (lba % 63) + 1;
        cyl       = (lba + 1 - sect) / (16 * 63);
        lba_io[0] = sect;
        lba_io[1] = (cyl >> 0) & 0xFF;
        lba_io[2] = (cyl >> 8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba + 1 - sect) % (16 * 63) / (63);
    }
    while (this->readReg(channel, ATA_REG_STATUS) & ATA_SR_BSY) {}
    if (lba_mode == 0)
        this->writeReg(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head);
    else
        this->writeReg(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head);
    if (lba_mode == 2) {
        this->writeReg(channel, ATA_REG_SECCOUNT1, 0);
        this->writeReg(channel, ATA_REG_LBA3, lba_io[3]);
        this->writeReg(channel, ATA_REG_LBA4, lba_io[4]);
        this->writeReg(channel, ATA_REG_LBA5, lba_io[5]);
    }
    this->writeReg(channel, ATA_REG_SECCOUNT0, (uint8_t)sectors);
    this->writeReg(channel, ATA_REG_LBA0, lba_io[0]);
    this->writeReg(channel, ATA_REG_LBA1, lba_io[1]);
    this->writeReg(channel, ATA_REG_LBA2, lba_io[2]);
    if (lba_mode == 0) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 1) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 2) cmd = ATA_CMD_WRITE_PIO_EXT;
    this->writeReg(channel, ATA_REG_COMMAND, cmd);
    for (i = 0; i < sectors; i++) {
        if (this->poll(channel, true)) {
            dbg::printm(MODULE, "Write to %hhu: %llu - %llu (%lu sectors) failed\n", drive, lba,
                        lba + sectors, sectors);
            std::abort();
        }
        __asm__ volatile("pushw %ax");
        __asm__ volatile("mov %ds, %ax");
        __asm__ volatile("pushw %ax");
        __asm__ volatile("mov %%ax, %%ds" : : "a"(0x10));
        __asm__ volatile("rep outsw" : : "c"(words), "d"(bus), "S"(buffer));
        __asm__ volatile("popw %ax");
        __asm__ volatile("mov %ax, %ds");
        __asm__ volatile("popw %ax");
        buffer = (void*)((uint8_t*)buffer + (words * 2));
    }
    this->writeReg(channel, ATA_REG_COMMAND,
                   (char[]){(int8_t)ATA_CMD_CACHE_FLUSH, (int8_t)ATA_CMD_CACHE_FLUSH,
                            (int8_t)ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
    this->poll(channel, false);
    dbg::popTrace();
    return true;
}
IDEDriver* loadIDEController(pci::device* device) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    IDEDriver* drv = new IDEDriver();
    assert(drv);
    drv->init(device);
    dbg::popTrace();
    return drv;
}
uint8_t IDEDriver::readReg(uint8_t channel, uint8_t reg) {
    uint8_t result = 0;
    if (reg > 0x07 && reg < 0x0C) {
        this->writeReg(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }
    if (reg < 0x08) {
        result = io::inb(channels[channel].base + reg - 0x00);
    } else if (reg < 0x0C) {
        result = io::inb(channels[channel].base + reg - 0x06);
    } else if (reg < 0x0E) {
        result = io::inb(channels[channel].ctrl + reg - 0x0A);
    } else if (reg < 0x16) {
        result = io::inb(channels[channel].bmide + reg - 0x0E);
    }
    if (reg > 0x07 && reg < 0x0C) {
        this->writeReg(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }
    return result;
}
void IDEDriver::writeReg(uint8_t channel, uint8_t reg, uint8_t data) {
    if (reg > 0x07 && reg < 0x0C) {
        this->writeReg(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }
    if (reg < 0x08) {
        io::outb(channels[channel].base + reg - 0x00, data);
    } else if (reg < 0x0C) {
        io::outb(channels[channel].base + reg - 0x06, data);
    } else if (reg < 0x0E) {
        io::outb(channels[channel].ctrl + reg - 0x0A, data);
    } else if (reg < 0x16) {
        io::outb(channels[channel].bmide + reg - 0x0E, data);
    }
    if (reg > 0x07 && reg < 0x0C) {
        this->writeReg(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }
}
void IDEDriver::readBuffer(uint8_t channel, uint8_t reg, void* buffer, uint32_t quads) {
    if (reg > 0x07 && reg < 0x0C) {
        this->writeReg(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    }
    __asm__ volatile("pushw %ax");
    __asm__ volatile("mov %es, %ax");
    __asm__ volatile("pushw %ax");
    __asm__ volatile("mov %ds, %ax");
    __asm__ volatile("mov %ax, %es");
    if (reg < 0x08) {
        io::insl(channels[channel].base + reg - 0x00, buffer, quads);
    } else if (reg < 0x0C) {
        io::insl(channels[channel].base + reg - 0x06, buffer, quads);
    } else if (reg < 0x0E) {
        io::insl(channels[channel].ctrl + reg - 0x0A, buffer, quads);
    } else if (reg < 0x16) {
        io::insl(channels[channel].bmide + reg - 0x0E, buffer, quads);
    }
    __asm__ volatile("popw %ax");
    __asm__ volatile("mov %ax, %es");
    __asm__ volatile("popw %ax");
    if (reg > 0x07 && reg < 0x0C) {
        this->writeReg(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    }
}
}; // namespace drivers::block