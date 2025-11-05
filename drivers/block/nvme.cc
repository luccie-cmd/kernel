#include <cassert>
#include <cmath>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <cstring>
#include <drivers/block/nvme.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <kernel/mmu/mmu.h>
#include <kernel/task/task.h>
#include <memory>
#define MODULE "NVMe Driver"

constexpr int64_t intpow(int64_t base, int64_t exp) {
    int64_t result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) result *= base; // If exponent is odd, multiply base
        base *= base;                     // Square the base
        exp /= 2;                         // Reduce exponent by half
    }
    return result;
}

uint64_t cmdId = 10;

namespace drivers::block {
NVMeDriver::NVMeDriver() : MSCDriver(StorageType::NVMe) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    this->nsid      = 1;
    this->base_addr = 0;
    this->diskSize  = 0;
    dbg::popTrace();
}
NVMeDriver::~NVMeDriver() {}
void NVMeDriver::deinit() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printm(MODULE, "TODO: Deinit NVMe driver\n");
    std::abort();
    dbg::popTrace();
}
void NVMeDriver::init(pci::device* device) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    pci::enableBusmaster(device);
    uint32_t BAR0   = pci::readConfig(device, 0x10);
    uint32_t BAR1   = pci::readConfig(device, 0x14);
    this->nsid      = this->__identifier_index;
    this->base_addr = (uint64_t)(((uint64_t)BAR1 << 32) | (BAR0 & 0xFFFFFFF0));
    mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), this->base_addr, this->base_addr,
                      PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW,
                      MAP_GLOBAL | MAP_PRESENT | MAP_UC);
    uint64_t cap = this->readReg64(0x00);
    std::memcpy(&this->capabilities, &cap, sizeof(cap));
    if (intpow(2, 12 + this->capabilities.MinPagesize) != PAGE_SIZE) {
        dbg::printm(MODULE, "Page sizes of 0x%llx not supported, minimum size: 0x%llx\n", PAGE_SIZE,
                    intpow(2, 12 + this->capabilities.MinPagesize));
        std::abort();
    }
    if (this->capabilities.DefaultCommandSet == 0) {
        dbg::printm(MODULE, "Default command set not supported\n");
        std::abort();
    }
    if (this->capabilities.CSS_AdminOnly == 1) {
        dbg::printm(MODULE, "Admin only commands are not supported\n");
        std::abort();
    }
    if (this->capabilities.CSS_MultipleIo == 0) {
        dbg::printm(MODULE, "I/O commands are not supported\n");
        std::abort();
    }
    if (this->capabilities.DoorbellStride != 0) {
        dbg::printm(MODULE, "TODO: Support DoorbellStride\n");
        std::abort();
    }
    uint32_t cc = this->readReg(0x14);
    cc &= (uint32_t)~1;
    this->writeReg(0x14, cc, false);
    while ((this->readReg(0x14) & 0x1) != 0) {
        __asm__("nop");
    }
    uint32_t csts = readReg(0x1C);
    if ((csts & 0x1) != 0) {
        dbg::printm(MODULE, "Failed to disable controller\n");
        std::abort();
    }
    size_t submissionQueueSize = this->capabilities.MaxQueueEntries * sizeof(NVMeCommand);
    size_t completionQueueSize = this->capabilities.MaxQueueEntries * sizeof(NVMeCompletion);
    this->admSq                = new NVMeQueue;
    this->admCq                = new NVMeQueue;
    this->admSq->addr          = mmu::pmm::allocVirtual(submissionQueueSize);
    this->admCq->addr          = mmu::pmm::allocVirtual(completionQueueSize);
    this->admSq->size          = this->capabilities.MaxQueueEntries;
    this->admCq->size          = this->capabilities.MaxQueueEntries;
    for (size_t i = 0; i < completionQueueSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(
            mmu::vmm::getPML4(KERNEL_PID), this->admCq->addr + i, this->admCq->addr + i,
            PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW, MAP_UC | MAP_PRESENT);
    }
    for (size_t i = 0; i < submissionQueueSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(
            mmu::vmm::getPML4(KERNEL_PID), this->admSq->addr + i, this->admSq->addr + i,
            PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW, MAP_UC | MAP_PRESENT);
    }
    std::memset((void*)this->admSq->addr, 0, submissionQueueSize);
    std::memset((void*)this->admCq->addr, 0, completionQueueSize);
    this->admCq->qid  = 0;
    this->admSq->qid  = 0;
    this->admCq->head = 0;
    this->admSq->tail = 0;
    this->writeReg64(0x28, this->admSq->addr, false);
    this->writeReg64(0x30, this->admCq->addr, false);
    this->writeReg(0x24, ((this->admCq->size - 1) << 16) | (this->admSq->size - 1), true);
    cc                  = this->readReg(0x14);
    uint32_t iosqes_val = 6;
    uint32_t iocqes_val = 4;
    cc |= (iosqes_val & 0x1F) << 16;
    cc |= (iocqes_val & 0x0F) << 20;
    cc |= 1;
    this->writeReg(0x14, cc, false);
    while ((this->readReg(0x1C) & 0x1) == 0) {
        __asm__("nop");
    }
    csts = this->readReg(0x1C);
    if ((csts & ~1) != 0) {
        dbg::printm(MODULE, "Controller status error bits are set: 0b%032b\n", csts);
        std::abort();
    }
    this->ioCq       = new NVMeQueue;
    this->ioSq       = new NVMeQueue;
    this->ioSq->addr = mmu::pmm::allocVirtual(submissionQueueSize);
    this->ioCq->addr = mmu::pmm::allocVirtual(completionQueueSize);
    for (size_t i = 0; i < completionQueueSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), this->ioCq->addr + i, this->ioCq->addr + i,
                          PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW,
                          MAP_UC | MAP_PRESENT);
    }
    for (size_t i = 0; i < submissionQueueSize; i += PAGE_SIZE) {
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), this->ioSq->addr + i, this->ioSq->addr + i,
                          PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW,
                          MAP_UC | MAP_PRESENT);
    }
    std::memset((void*)this->ioSq->addr, 0, submissionQueueSize);
    std::memset((void*)this->ioCq->addr, 0, completionQueueSize);
    this->ioSq->size = this->capabilities.MaxQueueEntries;
    this->ioCq->size = this->capabilities.MaxQueueEntries;
    this->ioCq->head = 0;
    this->ioSq->tail = 0;
    this->ioCq->qid  = 1;
    this->ioSq->qid  = 1;
    this->setupIOQueues();
    uint8_t* addr = (uint8_t*)mmu::pmm::allocate();
    mmu::vmm::mapPage((uint64_t)addr);
    std::memset(addr, 0, PAGE_SIZE);
    NVMeCommand* identifyCommand = new NVMeCommand;
    std::memset(identifyCommand, 0, sizeof(NVMeCommand));
    identifyCommand->prp1       = (uint64_t)addr;
    identifyCommand->command_id = cmdId++;
    identifyCommand->opcode     = 0x06;
    identifyCommand->cwd10      = 0x02;
    this->sendCmd(this->admSq, this->admCq, identifyCommand);
    this->nsid = 0;
    for (size_t i = 0; i < PAGE_SIZE; i += 4) {
        uint32_t curNsid = *(uint32_t*)(addr + i);
        if (curNsid == 0) {
            break;
        }
        if (this->nsid != 0) {
            dbg::printf("TODO: Support multiple NSIDs\n");
            std::abort();
        }
        this->nsid = curNsid;
    }
    std::memset(identifyCommand, 0, sizeof(NVMeCommand));
    identifyCommand->prp1       = (uint64_t)addr;
    identifyCommand->command_id = cmdId++;
    identifyCommand->opcode     = 0x06;
    identifyCommand->cwd10      = 0x00;
    identifyCommand->nsid       = this->nsid;
    this->sendCmd(this->admSq, this->admCq, identifyCommand);
    NVMeIdentifyNamespace identifyNamespace;
    std::memcpy(&identifyNamespace, addr, sizeof(NVMeIdentifyNamespace));
    if (identifyNamespace.lbafs[identifyNamespace.FLBAS].lbads != 9) {
        dbg::printm(MODULE, "TODO: Support non 512 blocks\n");
        std::abort();
    }
    this->diskSize = identifyNamespace.namespaceSize;
    dbg::popTrace();
}
void NVMeDriver::setupIOQueues() {
    NVMeCommand* createIOQueue = new NVMeCommand;
    std::memset(createIOQueue, 0, sizeof(NVMeCommand));
    createIOQueue->prp1       = this->ioCq->addr;
    createIOQueue->cwd10      = ((uint32_t)this->ioCq->qid) | ((uint32_t)this->ioCq->size << 16);
    createIOQueue->cwd11      = 1;
    createIOQueue->opcode     = 5;
    createIOQueue->command_id = cmdId++;
    this->sendCmd(this->admSq, this->admCq, createIOQueue);

    std::memset(createIOQueue, 0, sizeof(NVMeCommand));
    createIOQueue->prp1       = this->ioSq->addr;
    createIOQueue->cwd10      = ((uint32_t)this->ioSq->qid) | ((uint32_t)this->ioSq->size << 16);
    createIOQueue->cwd11      = ((uint32_t)this->ioCq->qid << 16) | 1;
    createIOQueue->opcode     = 1;
    createIOQueue->command_id = cmdId++;
    this->sendCmd(this->admSq, this->admCq, createIOQueue);
    delete createIOQueue;
}
bool NVMeDriver::sendCmd(NVMeQueue* sq, NVMeQueue* cq, NVMeCommand* cmd) {
    uint8_t* entry = (uint8_t*)sq->addr + (sq->tail * sizeof(NVMeCommand));
    std::memcpy((void*)entry, cmd, sizeof(NVMeCommand));
    __asm__ volatile("mfence" ::: "memory");
    sq->tail = (sq->tail + 1) % sq->size;
    this->writeReg(0x1000 + 8 * sq->qid, sq->tail, false);
    uint8_t  attempts = 255;
    uint16_t status   = 0;
    while (attempts) {
        volatile NVMeCompletion* cqe =
            (volatile NVMeCompletion*)(cq->addr + cq->head * sizeof(NVMeCompletion));
        uint32_t timeout = 500000;
        while (timeout) {
            __asm__("nop");
            timeout--;
        }
        __asm__ volatile("lfence" ::: "memory");
        status = cqe->status;
#ifdef DEBUG
        dbg::printm(
            MODULE, "CQE->status = %015.15b CQE->cmd_id = %x CQE->sq_id = %x condition = %s\n",
            cqe->status, cqe->cmd_id, cqe->sq_id,
            (cqe->status != 0x7FFF and cqe->cmd_id == cmd->command_id and cqe->sq_id == sq->qid)
                ? "true"
                : "false");
#endif
        if (cqe->status != 0x7FFF && cqe->cmd_id == cmd->command_id && cqe->sq_id == sq->qid) {
            break;
        }
        attempts--;
    }
    if (attempts == 0) {
        dbg::printm(MODULE, "Exhausted attempts\n");
        std::abort();
    }
    cq->head = (cq->head + 1) % cq->size;
    this->writeReg(0x1000 + 8 * cq->qid + 4, cq->head, false);
    return status == 0;
}
bool NVMeDriver::read(uint8_t drive, uint64_t lba, uint32_t sectors, volatile uint8_t* buffer) {
    if (drive != this->nsid - 1) {
        dbg::printm(MODULE, "Warning drive doesn't correspond to NSID\n");
    }
    dbg::addTrace(__PRETTY_FUNCTION__);
    NVMeCommand* cmd = new NVMeCommand;
    std::memset(cmd, 0, sizeof(NVMeCommand));
    uint64_t pageAlignBuffer = mmu::pmm::allocVirtual(sectors * 512);
    for (size_t i = 0; i < sectors * 512; i += PAGE_SIZE) {
        mmu::vmm::mapPage(pageAlignBuffer + i);
    }
    cmd->command_id = cmdId++;
    cmd->opcode     = 0x02;
    cmd->nsid       = this->nsid;
    cmd->prp1       = pageAlignBuffer;
    cmd->cwd10      = (uint32_t)lba;
    cmd->cwd11      = (uint32_t)((uint64_t)lba >> 32);
    cmd->cwd12      = (uint16_t)(sectors - 1);
    cmd->cwd13      = 2;
    bool result     = this->sendCmd(this->ioSq, this->ioCq, cmd);
    std::memcpy((void*)buffer, (void*)pageAlignBuffer, sectors * 512);
    for (size_t i = 0; i < sectors * 512; i += PAGE_SIZE) {
        mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), pageAlignBuffer + i);
    }
    mmu::pmm::free(pageAlignBuffer, sectors * 512);
    dbg::popTrace();
    return result;
}
bool NVMeDriver::write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer) {
    (void)lba;
    (void)sectors;
    (void)buffer;
    if (drive != this->nsid) {
        dbg::printm(MODULE, "Warning drive doesn't correspond to NSID\n");
    }
    __builtin_unreachable();
    // dbg::addTrace(__PRETTY_FUNCTION__);
    // bool result = this->sendCmdIO(0x01, buffer, lba, sectors);
    // dbg::popTrace();
    // return result;
}
uint32_t NVMeDriver::readReg(uint32_t offset) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    volatile uint32_t* reg = (volatile uint32_t*)(this->base_addr + offset);
    if (mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)reg, true, false) == 0) {
        mmu::vmm::mapPage((uint64_t)reg);
    }
    __asm__ volatile("mfence" ::: "memory");
    volatile uint32_t ret = *reg;
    forceReadVolatile(ret);
#ifdef DEBUG
    dbg::printm(MODULE, "r base addr: 0x%llx offset: 0x%llx reg: 0x%llx ret: 0x%llx\n",
                this->base_addr, offset, reg, ret);
#endif
    dbg::popTrace();
    return ret;
}
uint64_t NVMeDriver::readReg64(uint32_t offset) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    volatile uint64_t* reg = (volatile uint64_t*)(this->base_addr + offset);
    if (mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)reg, true, false) == 0) {
        mmu::vmm::mapPage((uint64_t)reg);
    }
    __asm__ volatile("mfence" ::: "memory");
    volatile uint64_t ret = *reg;
    forceReadVolatile(ret);
#ifdef DEBUG
    dbg::printm(MODULE, "r64 base addr: 0x%llx offset: 0x%llx reg: 0x%llx ret: 0x%llx\n",
                this->base_addr, offset, reg, ret);
#endif
    dbg::popTrace();
    return ret;
}
void NVMeDriver::writeReg(uint32_t offset, uint32_t value, bool check) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    volatile uint32_t* reg = (volatile uint32_t*)(this->base_addr + offset);
    if (mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)reg, true, false) == 0) {
        mmu::vmm::mapPage((uint64_t)reg);
    }
#ifdef DEBUG
    dbg::printm(MODULE, "w base addr: 0x%llx offset: 0x%llx reg: 0x%llx value: 0x%llx\n",
                this->base_addr, offset, reg, value);
#endif
    __asm__ volatile("mfence" ::: "memory");
    *reg = value;
    __asm__ volatile("sfence" ::: "memory");
    if (this->readReg(offset) != value && check) {
        dbg::printm(MODULE, "Failed to update value at 0x%llx (32 bit)\n", (uint64_t)reg);
        std::abort();
    }
    dbg::popTrace();
}
void NVMeDriver::writeReg64(uint32_t offset, uint64_t value, bool check) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    volatile uint64_t* reg = (volatile uint64_t*)(this->base_addr + offset);
    if (mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)reg, true, false) == 0) {
        mmu::vmm::mapPage((uint64_t)reg);
    }
#ifdef DEBUG
    dbg::printm(MODULE, "w64 base addr: 0x%llx offset: 0x%llx reg: 0x%llx value: 0x%llx\n",
                this->base_addr, offset, reg, value);
#endif
    __asm__ volatile("mfence" ::: "memory");
    *reg = value;
    __asm__ volatile("mfence" ::: "memory");
    if (this->readReg64(offset) != value && check) {
        dbg::printm(MODULE, "Failed to update value at 0x%llx (64 bit)\n", (uint64_t)reg);
        std::abort();
    }
    dbg::popTrace();
}
uint8_t NVMeDriver::getConnectedDrives() {
    return 1;
}
uint64_t NVMeDriver::getDiskSize(uint8_t disk) {
    (void)disk;
    return this->diskSize;
}
NVMeDriver* loadNVMeDriver(pci::device* device) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    NVMeDriver* drv = new NVMeDriver();
    assert(drv);
    drv->init(device);
    dbg::popTrace();
    return drv;
}
}; // namespace drivers::block