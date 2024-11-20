#if !defined(_DRIVERS_FS_H_)
#define _DRIVERS_FS_H_
#include <kernel/driver/driver.h>
#include <kernel/vfs/gpt.h>
#include <kernel/task/types.h>

namespace drivers{
    enum struct FSType{
        FAT32,
        EXT4,
    };
    class FSDriver : public driver::Driver{
        public:
            FSDriver(vfs::PartitionEntry* entry);
            virtual ~FSDriver() = 0;
            virtual void init(pci::device* device) = 0;
            virtual void deinit() = 0;
            virtual int open(task::pid_t PID, const char* path, int flags) = 0;
            virtual void read(int file, size_t length, void* buffer) = 0;
            virtual void close(int file) = 0;
            vfs::PartitionEntry* getPartEntry();
        private:
            vfs::PartitionEntry* __partition_entry;
            FSType __fs_type;
    };
    FSDriver* loadFSDriver(vfs::PartitionEntry* entry);
};

#endif // _DRIVERS_FS_H_
