#if !defined(_KERNEL_DRIVER_MSC_H_)
#define _KERNEL_DRIVER_MSC_H_
#include <kernel/driver/driver.h>
#define SECTOR_SIZE 512

namespace drivers{
    enum struct StorageType : int {
        IDE,
        NVMe,
    };
    class MSCDriver : public driver::Driver {
        public:
            MSCDriver(StorageType storage_type);
            virtual ~MSCDriver();
            virtual bool read(uint8_t drive, uint64_t lba, uint32_t sectors, volatile uint8_t* buffer) = 0;
            virtual bool write(uint8_t drive, uint64_t lba, uint32_t sectors, void* buffer) = 0;
            virtual void init(pci::device* device) = 0;
            virtual void deinit() = 0;
            virtual uint8_t getConnectedDrives() = 0;
            virtual uint64_t getDiskSize(uint8_t disk) = 0;
            void setVolumeName(const char* name);
        private:
            StorageType __storage_type;
            const char* __volume_name;
        protected:
            uint64_t __identifier_index;
    };
    uint64_t getNewIdentIndex();
    MSCDriver* loadMSCdriver(pci::device* device);
};

#endif // _KERNEL_DRIVER_MSC_H_
