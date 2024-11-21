#if !defined(_DRIVERS_FAT32_H_)
#define _DRIVERS_FAT32_H_
#include "fs.h"
#define FAT32_MAX_FILE_HANDLES       10
#define FAT32_CACHE_SIZE              5
#define FAT32_ROOT_DIRECTORY_HANDLE  -1

namespace drivers{
    struct FAT_ExtendedBootRecord{
        uint8_t DriveNumber;
        uint8_t _Reserved;
        uint8_t Signature;
        uint32_t VolumeId;
        uint8_t VolumeLabel[11];
        uint8_t SystemId[8];
    } __attribute__((packed));
    struct FAT32_ExtendedBootRecord{
        uint32_t SectorsPerFat;
        uint16_t Flags;
        uint16_t FatVersion;
        uint32_t RootDirectoryCluster;
        uint16_t FSInfoSector;
        uint16_t BackupBootSector;
        uint8_t _Reserved[12];
        FAT_ExtendedBootRecord EBR;
    } __attribute((packed));
    struct FAT_BootSector{
        uint8_t BootJumpInstruction[3];
        uint8_t OemIdentifier[8];
        uint16_t BytesPerSector;
        uint8_t SectorsPerCluster;
        uint16_t ReservedSectors;
        uint8_t FatCount;
        uint16_t DirEntryCount;
        uint16_t TotalSectors;
        uint8_t MediaDescriptorType;
        uint16_t SectorsPerFat;
        uint16_t SectorsPerTrack;
        uint16_t Heads;
        uint32_t HiddenSectors;
        uint32_t LargeSectorCount;
        union {
            FAT_ExtendedBootRecord EBR1216;
            FAT32_ExtendedBootRecord EBR32;
        };
        uint8_t reserved0[422];
    } __attribute__((packed));
    struct FAT_DirectoryEntry{
        uint8_t Name[11];
        uint8_t Attributes;
        uint8_t _Reserved;
        uint8_t CreatedTimeTenths;
        uint16_t CreatedTime;
        uint16_t CreatedDate;
        uint16_t AccessedDate;
        uint16_t FirstClusterHigh;
        uint16_t ModifiedTime;
        uint16_t ModifiedDate;
        uint16_t FirstClusterLow;
        uint32_t Size;
    } __attribute__((packed));
    struct FAT_File{
        int Handle;
        task::pid_t Pid;
        bool IsDirectory;
        uint32_t Position;
        uint32_t Size;
    };
    struct FAT_FileData{
        uint8_t Buffer[SECTOR_SIZE];
        FAT_File Public;
        bool Opened;
        uint32_t FirstCluster;
        uint32_t CurrentCluster;
        uint32_t CurrentSectorInCluster;
    };
    enum struct FAT_Attributes : int{
        READ_ONLY         = 0x01,
        HIDDEN            = 0x02,
        SYSTEM            = 0x04,
        VOLUME_ID         = 0x08,
        DIRECTORY         = 0x10,
        ARCHIVE           = 0x20,
    };
    class FAT32Driver : public FSDriver{
        public:
            FAT32Driver(vfs::PartitionEntry* entry, driver::MSCDriver* diskDevice);
            ~FAT32Driver();
            void init(pci::device* dev);
            void deinit();
            int open(task::pid_t PID, const char* path, int flags);
            void read(int file, size_t length, void* buffer);
            void close(int file);
        private:
            FAT_BootSector* bootSector;
            FAT_FileData* rootDir;
            FAT_FileData* openedFiles[FAT32_MAX_FILE_HANDLES];
            uint8_t cache[FAT32_CACHE_SIZE*SECTOR_SIZE];
            uint32_t cachePos;
            uint32_t maxSectors;
            uint32_t sectorsPerFat;
            uint32_t dataSectionLBA;
            uint32_t clusterToLBA(uint32_t cluster);
            bool findFile(FAT_File* file, char* name, FAT_DirectoryEntry* outEntry);
            FAT_File* openEntry(FAT_DirectoryEntry *entry);
            bool readEntry(FAT_File* file, FAT_DirectoryEntry* dirEntry);
            uint32_t nextCluster(uint32_t currentCluster);
            uint32_t readBytes(FAT_File* file, uint32_t byteCount, void* dataOut);
            void readFat(size_t lbaIdx);
    };
};

#endif // _DRIVERS_FAT32_H_
