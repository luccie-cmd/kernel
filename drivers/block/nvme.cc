#include <drivers/block/nvme.h>
#include <cstdlib>
#include <common/dbg/dbg.h>
#include <kernel/mmu/mmu.h>
#include <kernel/task/task.h>
#include <cmath>
#include <cstring>
#include <memory>
#include <cassert>
#define MODULE "NVMe Driver"

constexpr int64_t intpow(int64_t base, int64_t exp) {
    int64_t result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) result *= base;  // If exponent is odd, multiply base
        base *= base;  // Square the base
        exp /= 2;  // Reduce exponent by half
    }
    return result;
}

namespace drivers::block{
    NVMeDriver::NVMeDriver() :MSCDriver(StorageType::NVMe) {
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->nsid = 1;
        this->base_addr = 0;
        this->diskSize = 0;
        dbg::popTrace();
    }
    NVMeDriver::~NVMeDriver(){}
    void NVMeDriver::deinit(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        dbg::printm(MODULE, "TODO: Deinit NVMe driver\n");
        std::abort();
        dbg::popTrace();
    }
    void NVMeDriver::init(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        pci::enableBusmaster(device);
        uint32_t BAR0 = pci::readConfig(device, 0x10);
        uint32_t BAR1 = pci::readConfig(device, 0x14);
        this->base_addr = (uint64_t)(((uint64_t)BAR1 << 32) | (BAR0 & 0xFFFFFFF0));
        mmu::vmm::mapPage(mmu::vmm::getPML4(task::getCurrentPID()), this->base_addr, this->base_addr, PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW, MAP_GLOBAL | MAP_PRESENT | MAP_UC);
        uint64_t cap = this->readReg64(0x00);
        *(uint64_t*)&this->capabilities = cap;
        if(intpow(2, 12+this->capabilities.MinPagesize) != PAGE_SIZE){
            dbg::printm(MODULE, "Page sizes of 0x%llx not supported, minimum size: 0x%llx\n", PAGE_SIZE, intpow(2, 12+this->capabilities.MinPagesize));
            std::abort();
        }
        if(this->capabilities.DefaultCommandSet == 0){
            dbg::printm(MODULE, "Default command set not supported\n");
            std::abort();
        }
        if(this->capabilities.CSS_AdminOnly == 0){
            dbg::printm(MODULE, "Admin only commands are not supported\n");
            std::abort();
        }
        uint32_t cc = this->readReg(0x14);
        cc &= (uint32_t)~1;
        this->writeReg(0x14, cc, false);
        while ((this->readReg(0x14) & 0x1) != 0){
            __asm__("nop");
        }
        uint32_t csts = readReg(0x1C);
        if ((csts & 0x1) != 0) {
            dbg::printm(MODULE, "Failed to disable controller\n");
            std::abort();
        }
        this->admSq = new NVMeQueue;
        this->admCq = new NVMeQueue;
        this->admSq->addr = mmu::pmm::allocVirtual((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCommand));
        this->admCq->addr = mmu::pmm::allocVirtual((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCompletion));
        this->admSq->size = this->capabilities.MaxQueueEntries;
        this->admCq->size = this->capabilities.MaxQueueEntries;
        this->admCq_head = 0;
        this->admSq_tail = 0;
        this->writeReg64(0x28, this->admSq->addr, true);
        this->writeReg64(0x30, this->admCq->addr, true);
        this->writeReg(0x24, (this->admCq->size << 16) | this->admSq->size, true);
        cc = this->readReg(0x14);
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
        this->ioCq = new NVMeQueue;
        this->ioSq = new NVMeQueue;
        this->ioSq->addr = mmu::pmm::allocVirtual((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCommand));
        this->ioCq->addr = mmu::pmm::allocVirtual((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCompletion));
        this->ioSq->size = this->capabilities.MaxQueueEntries;
        this->ioCq->size = this->capabilities.MaxQueueEntries;
        std::memset((void*)this->ioSq->addr, 0, (this->ioSq->size+1) * sizeof(NVMeCommand));
        std::memset((void*)this->ioCq->addr, 0, (this->ioCq->size+1) * sizeof(NVMeCompletion));
        this->sendCmdADM(1, (void*)this->ioSq->addr, this->ioSq->size | (1 << 16), (1 << 0), 0);
        this->sendCmdADM(5, (void*)this->ioCq->addr, this->ioCq->size | (1 << 16), (1 << 0), 0);
        uint8_t* addr = (uint8_t*)mmu::pmm::allocate();
        std::memset(addr, 0, PAGE_SIZE);
        this->sendCmdADM(6, (void*)addr, 1, 0, 0);
        for(size_t i = 0; i < PAGE_SIZE; ++i){
            dbg::printf("%02x ", addr[i]);
            if((i+1)%32 == 0){
                dbg::print("\n");
            }
        }
        dbg::popTrace();
    }
    bool NVMeDriver::read(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer){
        if(drive != this->nsid){
            dbg::printm(MODULE, "Warning drive doesn't correspond to NSID\n");
        }
        dbg::addTrace(__PRETTY_FUNCTION__);
        bool result = this->sendCmdIO(0x02, buffer, lba, sectors);
        dbg::popTrace();
        return result;
    }
    bool NVMeDriver::write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer){
        if(drive != this->nsid){
            dbg::printm(MODULE, "Warning drive doesn't correspond to NSID\n");
        }
        dbg::addTrace(__PRETTY_FUNCTION__);
        bool result = this->sendCmdIO(0x01, buffer, lba, sectors);
        dbg::popTrace();
        return result;
    }
    uint32_t NVMeDriver::readReg(uint32_t offset){
        dbg::addTrace(__PRETTY_FUNCTION__);
        volatile uint32_t *reg = (volatile uint32_t *)(this->base_addr + offset);
        __asm__ volatile("mfence" ::: "memory");
        uint32_t ret = *reg;
        dbg::printm(MODULE, "r base addr: 0x%llx offset: 0x%llx reg: 0x%llx ret: 0x%llx\n", this->base_addr, offset, reg, ret);
        dbg::popTrace();
        return ret;
    }
    uint64_t NVMeDriver::readReg64(uint32_t offset){
        dbg::addTrace(__PRETTY_FUNCTION__);
        volatile uint64_t *reg = (volatile uint64_t *)(this->base_addr + offset);
        __asm__ volatile("mfence" ::: "memory");
        uint64_t ret = *reg;
        dbg::printm(MODULE, "r64 base addr: 0x%llx offset: 0x%llx reg: 0x%llx ret: 0x%llx\n", this->base_addr, offset, reg, ret);
        dbg::popTrace();
        return ret;
    }
    void NVMeDriver::writeReg(uint32_t offset, uint32_t value, bool check){
        dbg::addTrace(__PRETTY_FUNCTION__);
        volatile uint32_t *reg = (volatile uint32_t *)(this->base_addr + offset);
        dbg::printm(MODULE, "w base addr: 0x%llx offset: 0x%llx reg: 0x%llx value: 0x%llx\n", this->base_addr, offset, reg, value);
        __asm__ volatile("sfence" ::: "memory");
        *reg = value;
        if(this->readReg(offset) != value && check){
            dbg::printm(MODULE, "Failed to update value at 0x%llx (32 bit)\n", (uint64_t)reg);
            std::exit(1);
        }
        dbg::popTrace();
    }
    void NVMeDriver::writeReg64(uint32_t offset, uint64_t value, bool check){
        dbg::addTrace(__PRETTY_FUNCTION__);
        volatile uint64_t *reg = (volatile uint64_t *)(this->base_addr + offset);
        dbg::printm(MODULE, "w64 base addr: 0x%llx offset: 0x%llx reg: 0x%llx value: 0x%llx\n", this->base_addr, offset, reg, value);
        __asm__ volatile("sfence" ::: "memory");
        *reg = value;
        if(this->readReg64(offset) != value && check){
            dbg::printm(MODULE, "Failed to update value at 0x%llx (64 bit)\n", (uint64_t)reg);
            std::exit(1);
        }
        dbg::popTrace();
    }
    uint8_t NVMeDriver::getConnectedDrives(){
        return 1;
    }
    uint64_t NVMeDriver::getDiskSize(uint8_t disk){
        (void)disk;
        return this->diskSize;
    }
    bool NVMeDriver::sendCmdIO(uint8_t opcode, void *data, uint64_t lba, uint16_t num_blocks) {
        dbg::addTrace(__PRETTY_FUNCTION__);
        void* pageAllignedBuffer = (void*)mmu::pmm::allocVirtual(num_blocks*SECTOR_SIZE);
        if(num_blocks*SECTOR_SIZE > PAGE_SIZE){
            dbg::printm(MODULE, "TODO: Support multiple 4096 byte blocks reading\n");
            std::abort();
        }
        std::memset(pageAllignedBuffer, 0, num_blocks*SECTOR_SIZE);
    	uint64_t sq_entry_addr = this->ioSq->addr + (this->ioSq_tail * sizeof(NVMeCommand));
    	uint64_t cq_entry_addr = this->ioCq->addr + (this->ioCq_head * sizeof(NVMeCompletion));
        NVMeCommand* cmd = new NVMeCommand;
        if(cmd == nullptr){
            dbg::printm(MODULE, "Failed to allocate NVMe command\n");
            std::abort();
        }
        cmd->command_id = 0x1337;
    	cmd->opcode = opcode;
    	cmd->nsid = this->nsid;
    	cmd->prp1 = mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(task::getCurrentPID()), (uint64_t)pageAllignedBuffer, false);
    	cmd->prp2 = 0;
    	cmd->cwd10 = (uint32_t)lba;
    	cmd->cwd11 = (uint32_t)((uint64_t)lba >> 32);
    	cmd->cwd12 = (uint16_t)(num_blocks-1);
    	std::memcpy((void *)sq_entry_addr, cmd, sizeof(NVMeCommand));
    	this->ioSq_tail++;
    	this->writeReg(0x1000 + 2 * (4 << this->capabilities.DoorbellStride), this->ioSq_tail, true);
    	if (this->ioSq_tail == this->ioSq->size)
    		this->ioSq_tail = 0;
        NVMeCompletion *completion = (NVMeCompletion *)(cq_entry_addr);
        if(mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(task::getCurrentPID()), cq_entry_addr, true) != cq_entry_addr){
            mmu::vmm::mapPage(cq_entry_addr);
        }
        while (true) {
	    	if (completion->cmd_id == cmd->command_id) {
                if(completion->cmd_id == 0){
                    continue;
                }
	    		break;
	    	}
	    }
    	this->ioCq_head++;
    	this->writeReg(0x1000 + (3 * (4 << this->capabilities.DoorbellStride)), this->ioCq_head, true);
    	if (this->ioCq_head == this->ioCq->size)
    		this->ioCq_head = 0;
        uint16_t status = completion->status;
        status &= 0x7FFF;
        delete cmd;
        if (status != 0) {
            dbg::printm(MODULE, "I/O command opcode 0x%x failed: status=0x%x\n", opcode, status);
        }
        std::memcpy(data, pageAllignedBuffer, num_blocks*SECTOR_SIZE);
        dbg::popTrace();
    	return status == 0;
    }
    bool NVMeDriver::sendCmdADM(uint8_t opcode, void* addr, uint32_t cwd10, uint32_t cwd11, uint32_t cwd1){
        dbg::addTrace(__PRETTY_FUNCTION__);
        void* pageAllignedBuffer = (void*)mmu::pmm::allocate();
        if(mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(task::getCurrentPID()), (uint64_t)pageAllignedBuffer, true) == 0){
            mmu::vmm::mapPage((size_t)pageAllignedBuffer);
        }
        std::memset(pageAllignedBuffer, 0, PAGE_SIZE);
    	uint64_t sq_entry_addr = this->admSq->addr + (this->admSq_tail * sizeof(NVMeCommand));
    	uint64_t cq_entry_addr = this->admCq->addr + (this->admCq_head * sizeof(NVMeCompletion));
        NVMeCommand *cmd = new NVMeCommand;
        if(cmd == nullptr){
            dbg::printm(MODULE, "Failed to allocate NVMe command\n");
            std::abort();
        }
        cmd->command_id = 0x1337;
    	cmd->opcode = opcode;
    	cmd->nsid = cwd1;
    	cmd->prp1 = (uint64_t)pageAllignedBuffer;
        cmd->prp2 = 0;
    	cmd->cwd10 = cwd10;
    	cmd->cwd11 = cwd11;
    	std::memcpy((void *)sq_entry_addr, cmd, sizeof(NVMeCommand));
    	this->admSq_tail++;
    	this->writeReg(0x1000 + 2 * (4 << this->capabilities.DoorbellStride), this->admSq_tail, true);
        uint32_t admSqTail = this->readReg(0x1000 + 2 * (4 << this->capabilities.DoorbellStride));
        if(this->admSq_tail != admSqTail){
            uint32_t status = this->readReg(0x1C);
            dbg::printm(MODULE, "Submission queue tail not acknowledged\nStatus: 0b%032lb\n", status);
            std::abort();
        }
    	if (this->admSq_tail == this->admSq->size)
    		this->admSq_tail = 0;
        NVMeCompletion *completion = (NVMeCompletion *)(cq_entry_addr);
        if(mmu::vmm::getPhysicalAddr(mmu::vmm::getPML4(task::getCurrentPID()), cq_entry_addr, true) != cq_entry_addr){
            mmu::vmm::mapPage(cq_entry_addr);
        }
        uint64_t timeout = this->capabilities.TimeoutMax;
        while (timeout > 0) {
            if (completion->cmd_id == cmd->command_id || timeout == 0) {
                break;
            }
            timeout--;
        }
        if (timeout == 0) {
            dbg::printm(MODULE, "Timeout waiting for completion\n");
            std::abort();
        }
    	this->admCq_head++;
    	this->writeReg(0x1000 + (3 * (4 << this->capabilities.DoorbellStride)), this->admCq_head, true);
    	if (this->admCq_head == this->admCq->size)
    		this->admCq_head = 0;
        uint16_t status = completion->status;
        if (status != 0) {
            dbg::printm(MODULE, "Admin command opcode 0x%x failed: status=0x%x\n", opcode, status);
        }
        std::memcpy(addr, pageAllignedBuffer, PAGE_SIZE);
        delete cmd;
        dbg::popTrace();
    	return status == 0;
    }
    NVMeDriver* loadNVMeDriver(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        NVMeDriver* drv = new (std::nothrow) NVMeDriver;
        assert(drv);
        drv->init(device);
        dbg::popTrace();
        return drv;
    }
};