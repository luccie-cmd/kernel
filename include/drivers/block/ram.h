#if !defined(_DRIVERS_BLOCK_RAM_H_)
#define _DRIVERS_BLOCK_RAM_H_
#include <cstdint>
#include <cstring>
#include <drivers/msc.h>

namespace drivers::block {
class RAMDriver : public MSCDriver {
  public:
    RAMDriver() : MSCDriver(StorageType::RAM) {}
    ~RAMDriver() {}
    bool read(uint8_t drive, uint64_t lba, uint32_t sectors, volatile uint8_t* buffer) {
        (void)drive;
        std::memcpy((void*)buffer, (uint8_t*)this->addr + lba * 512, sectors * 512);
        return true;
    }
    bool write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer) {
        (void)drive;
        std::memcpy((uint8_t*)this->addr + lba * 512, buffer, sectors * 512);
        return true;
    }
    void init(pci::device* device) {
        this->addr = (void*)device;
    }
    void    deinit() {}
    uint8_t getConnectedDrives() {
        return 1;
    }
    uint64_t getDiskSize(uint8_t disk) {
        (void)disk;
        return size;
    }
    void setSize(size_t _size) {
        this->size = _size;
    }

  private:
    void*  addr;
    size_t size;
};
} // namespace drivers::block

#endif // _DRIVERS_BLOCK_RAM_H_
