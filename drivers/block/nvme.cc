#include <drivers/block/nvme.h>
#include <cstdlib>
#include <common/dbg/dbg.h>
#include <kernel/mmu/mmu.h>
#include <kernel/task/task.h>
#include <cmath>
#include <cstring>
#include <cinttypes>
#define MODULE "NVMe Driver"

namespace drivers::block{
    NVMeDriver::NVMeDriver() :MSCDriver(StorageType::NVMe) {
        dbg::addTrace(__PRETTY_FUNCTION__);
        this->nsid = 1;
        this->cap_stride = 0;
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
        this->cap_stride = ((uint64_t)this->base_addr >> 12) & 0xF;
        uint64_t val = this->readReg64(0x00);
        *(uint64_t*)&this->capabilities = val;
        if(pow(2, 12+this->capabilities.MinPagesize) != PAGE_SIZE){
            dbg::printm(MODULE, "ERROR: Page sizes of 0x%llx not supported, minimum size: 0x%llx\n", PAGE_SIZE, pow(2, 12+this->capabilities.MinPagesize));
            std::abort();
        }
        if(this->capabilities.DefaultCommandSet == 0){
            dbg::printm(MODULE, "ERROR: Default command set not supported\n");
            std::abort();
        }
        if(this->capabilities.CSS_AdminOnly == 0){
            dbg::printm(MODULE, "ERROR: Admin only commands are not supported\n");
            std::abort();
        }
        uint32_t cc = this->readReg(0x14);
        cc &= ~1;
        this->writeReg(0x14, cc);
        while ((this->readReg(0x14) & 0x1) != 0){
            __asm__("nop");
        }
        uint32_t csts = readReg(0x1C);
        if ((csts & 0x1) != 0) {
            dbg::printm(MODULE, "Failed to disable controller\n");
            std::abort();
        }
        this->writeReg(0x14, 0);
        this->admSq = new NVMeQueue;
        this->admCq = new NVMeQueue;
        this->admSq->addr = mmu::pmm::allocVirtual((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCommand));
        this->admCq->addr = mmu::pmm::allocVirtual((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCompletion));
        this->admSq->size = this->capabilities.MaxQueueEntries;
        this->admCq->size = this->capabilities.MaxQueueEntries;
        for(size_t sqPgOffset = 0; sqPgOffset < ((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCommand)); sqPgOffset+=PAGE_SIZE){
            mmu::vmm::mapPage(this->admSq->addr+sqPgOffset);
        }
        for(size_t cqPgOffset = 0; cqPgOffset < ((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCompletion)); cqPgOffset+=PAGE_SIZE){
            mmu::vmm::mapPage(this->admCq->addr+cqPgOffset);
        }
        std::memset((void*)this->admSq->addr, 0, this->admSq->size * sizeof(NVMeCommand));
        std::memset((void*)this->admCq->addr, 0, this->admCq->size * sizeof(NVMeCompletion));
        this->admCq_head = 0;
        this->admSq_tail = 0;
        writeReg64(0x28, this->admSq->addr);
        writeReg64(0x30, this->admCq->addr);
        writeReg(0x24, (this->capabilities.MaxQueueEntries << 16) | this->capabilities.MaxQueueEntries);
        this->ioCq = new NVMeQueue;
        this->ioSq = new NVMeQueue;
        this->ioSq->addr = mmu::pmm::allocVirtual(this->capabilities.MaxQueueEntries * sizeof(NVMeCommand));
        this->ioCq->addr = mmu::pmm::allocVirtual(this->capabilities.MaxQueueEntries * sizeof(NVMeCompletion));
        this->ioSq->size = this->capabilities.MaxQueueEntries;
        this->ioCq->size = this->capabilities.MaxQueueEntries;
        for(size_t sqPgOffset = 0; sqPgOffset < ((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCommand)); sqPgOffset+=PAGE_SIZE){
            mmu::vmm::mapPage(this->ioSq->addr+sqPgOffset);
        }
        for(size_t cqPgOffset = 0; cqPgOffset < ((this->capabilities.MaxQueueEntries+1) * sizeof(NVMeCompletion)); cqPgOffset+=PAGE_SIZE){
            mmu::vmm::mapPage(this->ioCq->addr+cqPgOffset);
        }
        dbg::printm(MODULE, "Creating of submission queue status: %u\n", this->sendCmdADM(1, (void*)this->ioSq->addr, (this->ioSq->size - 1) | (1 << 16), (1 << 16), 0));
        dbg::printm(MODULE, "Creating of completion queue status: %u\n", this->sendCmdADM(5, (void*)this->ioCq->addr, (this->ioCq->size - 1) | (1 << 16), 0, 0));
        dbg::popTrace();
    }
    bool NVMeDriver::read(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer){
        (void)drive;
        dbg::addTrace(__PRETTY_FUNCTION__);
        bool failed = this->sendCmdIO(0x02, buffer, lba, sectors);
        dbg::popTrace();
        return !failed;
    }
    bool NVMeDriver::write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer){
        (void)drive;
        dbg::addTrace(__PRETTY_FUNCTION__);
        bool failed = this->sendCmdIO(0x01, buffer, lba, sectors);
        dbg::popTrace();
        return !failed;
    }
    uint32_t NVMeDriver::readReg(uint32_t offset){
        volatile uint32_t *reg = (volatile uint32_t *)(this->base_addr + offset);
	    mmu::vmm::mapPage((uint64_t)reg);
        return *reg;
    }
    uint64_t NVMeDriver::readReg64(uint32_t offset){
        volatile uint64_t *reg = (volatile uint64_t *)(this->base_addr + offset);
	    mmu::vmm::mapPage((uint64_t)reg);
        return *reg;
    }
    void NVMeDriver::writeReg(uint32_t offset, uint32_t value){
        volatile uint32_t *reg = (volatile uint32_t *)(this->base_addr + offset);
	    mmu::vmm::mapPage((uint64_t)reg);
        *reg = value;
    }
    void NVMeDriver::writeReg64(uint32_t offset, uint64_t value){
        volatile uint64_t *reg = (volatile uint64_t *)(this->base_addr + offset);
	    mmu::vmm::mapPage((uint64_t)reg);
        *reg = value;
    }
    uint8_t NVMeDriver::getConnectedDrives(){
        return 1;
    }
    uint64_t NVMeDriver::getDiskSize(uint8_t disk){
        return this->diskSize;
    }
    bool NVMeDriver::sendCmdIO(uint8_t opcode, void *data, uint64_t lba, uint16_t num_blocks) {
        dbg::addTrace(__PRETTY_FUNCTION__);
    	uint64_t sq_entry_addr = this->ioSq->addr + (this->ioSq_tail * sizeof(NVMeCommand));
    	uint64_t cq_entry_addr = this->ioCq->addr + (this->ioCq_head * sizeof(NVMeCompletion));
        NVMeCommand *cmd = new NVMeCommand;
        cmd->command_id = 1;
    	cmd->opcode = opcode;
    	cmd->nsid = this->nsid;
    	cmd->prp1 = (uintptr_t)data;
    	cmd->prp2 = 0;
    	cmd->cwd10 = (uint32_t)lba;
    	cmd->cwd11 = (uint32_t)((uint64_t)lba >> 32);
    	cmd->cwd12 = (uint16_t)(num_blocks - 1);
    	std::memcpy((void *)sq_entry_addr, cmd, sizeof(NVMeCommand));
    	this->ioSq_tail++;
    	this->writeReg(0x1000 + 2 * (4 << this->cap_stride), this->ioSq_tail);
    	if (this->ioSq_tail == this->ioSq->size)
    		this->ioSq_tail = 0;
        NVMeCompletion *completion = (NVMeCompletion *)(cq_entry_addr);
        while (true) {
	    	if (completion->cmd_id == cmd->command_id) {
	    		break;
	    	}
	    }
    	this->ioCq_head++;
    	this->writeReg(0x1000 + 3 * (4 << this->cap_stride), this->ioCq_head);
    	if (this->ioCq_head == this->ioCq->size)
    		this->ioCq_head = 0;
        uint16_t status = completion->status;
        status &= 0x7FFF;
        if (status != 0) {
            dbg::printm(MODULE, "Command failed: status=0x%x\n", status);
            std::abort();
        }
        delete cmd;
        dbg::popTrace();
    	return status != 0;
    }
    bool NVMeDriver::sendCmdADM(uint8_t opcode, void* addr, uint32_t cwd10, uint32_t cwd11, uint32_t cwd1){
        dbg::addTrace(__PRETTY_FUNCTION__);
    	uint64_t sq_entry_addr = this->admSq->addr + (this->admSq_tail * sizeof(NVMeCommand));
    	uint64_t cq_entry_addr = this->admCq->addr + (this->admCq_head * sizeof(NVMeCompletion));
        NVMeCommand *cmd = new NVMeCommand;
        cmd->command_id = 1;
    	cmd->opcode = opcode;
    	cmd->nsid = cwd1;
    	cmd->prp1 = (uintptr_t)addr;
    	cmd->prp2 = 0;
    	cmd->cwd10 = cwd10;
    	cmd->cwd11 = cwd11;
    	std::memcpy((void *)sq_entry_addr, cmd, sizeof(NVMeCommand));
    	this->admSq_tail++;
    	this->writeReg(0x1000 + 2 * (4 << this->cap_stride), this->admSq_tail);
    	if (this->admSq_tail == this->admSq->size)
    		this->admSq_tail = 0;
        NVMeCompletion *completion = (NVMeCompletion *)(this->admCq->addr + (this->admCq_head * sizeof(NVMeCompletion)));
        while (true) {
	    	if (completion->cmd_id == cmd->command_id) {
	    		break;
	    	}
	    }
    	this->admCq_head++;
    	this->writeReg(0x1000 + 3 * (4 << this->cap_stride), this->admCq_head);
    	if (this->admCq_head == this->admCq->size)
    		this->admCq_head = 0;
        uint16_t status = completion->status;
        status &= 0x7FFF;
        if (status != 0) {
            dbg::printm(MODULE, "Command failed: status=0x%x\n", status);
            std::abort();
        }
        delete cmd;
        dbg::popTrace();
    	return status != 0;
    }
    NVMeDriver* loadNVMeDriver(pci::device* device){
        dbg::addTrace(__PRETTY_FUNCTION__);
        NVMeDriver* drv = new NVMeDriver;
        drv->init(device);
        dbg::popTrace();
        return drv;
    }
};