#if !defined(_DRIVERS_BLOCK_NVME_H_)
#define _DRIVERS_BLOCK_NVME_H_
#include <drivers/msc.h>
#include <cstdint>

namespace drivers::block{
    struct NVMeCap {
        uint64_t MaxQueueEntries : 16;
        uint64_t PhysicallyContiguous : 1;
        uint64_t AMS_WeightedRoundRobinWithUrgent : 1;
        uint64_t AMS_VendorSpecific : 1;
        uint64_t Reserved0 : 5;
        uint64_t TimeoutMax : 8;
        uint64_t DoorbellStride : 4;
        uint64_t SubsystemReset : 1;
        uint64_t DefaultCommandSet : 1;
        uint64_t CSS_Reserved0 : 1;
        uint64_t CSS_Reserved1 : 1;
        uint64_t CSS_Reserved2 : 1;
        uint64_t CSS_Reserved3 : 1;
        uint64_t CSS_Reserved4 : 1;
        uint64_t CSS_MultipleIo : 1;
        uint64_t CSS_AdminOnly : 1;
        uint64_t Reserved2 : 3;
        uint64_t MinPagesize : 4;
        uint64_t MaxPagesize : 4;
        uint64_t Reserved3 : 8;
    };
    struct NVMeCommand {
        uint8_t opcode;
        uint8_t flags;
        uint16_t command_id;
        uint32_t nsid;
        uint64_t reserved;
        uint64_t mdPtr;
        uint64_t prp1;
        uint64_t prp2;
        uint32_t cwd10;
        uint32_t cwd11;
        uint32_t cwd12;
        uint32_t cwd13;
        uint32_t cwd14;
        uint32_t cwd15;
    } __attribute__((packed));
    static_assert(sizeof(NVMeCommand) == 64, "NVMe command improperely alligned");
    struct NVMeCompletion{
        uint32_t command;
        uint32_t reserved;
        uint16_t sq_head;
        uint16_t sq_id;
        uint16_t cmd_id;
        uint8_t phase : 1;
        uint16_t status : 15;
    } __attribute__((packed));
    static_assert(sizeof(NVMeCompletion) == 16, "NVMe completion improperely alligned");
    struct NVMeQueue{
        uint64_t addr;
        uint64_t size;
    };
    class NVMeDriver : public MSCDriver{
        public:
            NVMeDriver();
            ~NVMeDriver();
            bool read(uint8_t drive, uint64_t lba, uint32_t sectors, volatile uint8_t* buffer);
            bool write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer);
            void init(pci::device* device);
            void deinit();
            uint8_t getConnectedDrives();
            uint64_t getDiskSize(uint8_t disk);
        private:
            uint64_t base_addr;
            uint64_t nsid;
            uint64_t diskSize;
            NVMeCap capabilities;
            NVMeQueue* ioCq;
            NVMeQueue* ioSq;
            NVMeQueue* admCq;
            NVMeQueue* admSq;
            uint64_t ioSq_tail, ioCq_head, admSq_tail, admCq_head;
            bool sendCmdIO(uint8_t opcode, void *data, uint64_t lba, uint16_t num_blocks);
            bool sendCmdADM(uint8_t opcode, void* addr, uint32_t cwd10, uint32_t cwd11, uint32_t cwd1);
            uint32_t readReg(uint32_t offset);
            uint64_t readReg64(uint32_t offset);
            void writeReg(uint32_t offset, uint32_t value, bool check);
            void writeReg64(uint32_t offset, uint64_t value, bool check);
    };
    NVMeDriver* loadNVMeDriver(pci::device* device);
};

#endif // _DRIVERS_BLOCK_NVME_H_
