#if !defined(_DRIVERS_BLOCK_NVME_H_)
#define _DRIVERS_BLOCK_NVME_H_
#include <cstdint>
#include <drivers/msc.h>

namespace drivers::block {
struct __attribute__((packed)) NVMeCap {
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
static_assert(sizeof(NVMeCap) == 8);
struct NVMeCommand {
    uint8_t  opcode;
    uint8_t  flags;
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
struct NVMeCompletion {
    uint32_t command;
    uint32_t reserved;
    uint16_t sq_head;
    uint16_t sq_id;
    uint16_t cmd_id;
    uint8_t  phase : 1;
    uint16_t status : 15;
} __attribute__((packed));
static_assert(sizeof(NVMeCompletion) == 16, "NVMe completion improperely alligned");
struct NVMeQueue {
    uint64_t addr;
    uint64_t size;
    uint64_t head;
    uint64_t tail;
    uint64_t qid;
};
struct __attribute__((packed)) NVMeLbaf {
    uint16_t metaSize;
    uint8_t  lbads;
    uint8_t  rp;
};
struct NVMeIdentifyNamespace {
    uint64_t namespaceSize;
    uint64_t namespaceCapacity;
    uint64_t namespaceUtilization;
    uint8_t  namespaceFeatures;
    uint8_t  nLBAF;
    uint8_t  FLBAS;
    uint8_t  metadataCap;
    uint8_t  dpc;
    uint8_t  dps;
    uint8_t  reserved1[98];
    NVMeLbaf lbafs[16];
    uint8_t  reserved2[3904];
};
static_assert(sizeof(NVMeIdentifyNamespace) == 4096, "NVMeIdentifyNamespace not 4096 bytes");
class NVMeDriver : public MSCDriver {
  public:
    NVMeDriver();
    ~NVMeDriver();
    bool     read(uint8_t drive, uint64_t lba, uint32_t sectors, volatile uint8_t* buffer);
    bool     write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer);
    void     init(pci::device* device);
    void     deinit();
    uint8_t  getConnectedDrives();
    uint64_t getDiskSize(uint8_t disk);

  private:
    uint64_t   base_addr;
    uint64_t   nsid;
    uint64_t   diskSize;
    NVMeCap    capabilities;
    NVMeQueue* ioCq;
    NVMeQueue* ioSq;
    NVMeQueue* admCq;
    NVMeQueue* admSq;
    void       createIOQueues();
    bool       sendCmd(NVMeQueue* sq, NVMeQueue* cq, NVMeCommand* cmd);
    uint32_t   readReg(uint32_t offset);
    uint64_t   readReg64(uint32_t offset);
    void       writeReg(uint32_t offset, uint32_t value, bool check);
    void       writeReg64(uint32_t offset, uint64_t value, bool check);
    void       setupIOQueues();
};
NVMeDriver* loadNVMeDriver(pci::device* device);
}; // namespace drivers::block

#endif // _DRIVERS_BLOCK_NVME_H_
