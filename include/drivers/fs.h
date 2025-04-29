#if !defined(_DRIVERS_FS_H_)
#define _DRIVERS_FS_H_
#include <kernel/driver/driver.h>
#include <kernel/vfs/gpt.h>
#include <kernel/task/types.h>
#include <drivers/msc.h>

#define FS_FLAGS_READ (1 << 0)
#define SECTOR_SIZE 512

namespace drivers{
    enum struct FSType{
        FAT32,
        EXT2,
        EXT3,
        EXT4,
    };
    class FSDriver : public driver::Driver{
        public:
            FSDriver(vfs::PartitionEntry* entry, std::pair<MSCDriver*, uint8_t> drvDisk);
            virtual ~FSDriver() = 0;
            virtual void init(pci::device* dev) = 0;
            virtual void deinit() = 0;
            virtual int open(task::pid_t PID, const char* path, int flags) = 0;
            virtual void read(int file, size_t length, void* buffer) = 0;
            virtual void close(int file) = 0;
            virtual int getLengthOfFile(int file) = 0;
            vfs::PartitionEntry* getPartEntry();
            std::pair<MSCDriver*, uint8_t> getDiskDevice();
            FSType getFsType();
        private:
            vfs::PartitionEntry* __partition_entry;
            std::pair<MSCDriver*, uint8_t> __diskDevice;
        protected:
            FSType __fs_type;
    };
    FSDriver* loadFSDriver(vfs::PartitionEntry* entry, std::pair<MSCDriver*, uint8_t> drvDisk);
};

#endif // _DRIVERS_FS_H_
