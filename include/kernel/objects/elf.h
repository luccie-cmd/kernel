#if !defined(_KERNEL_OBJECTS_ELF_H_)
#define _KERNEL_OBJECTS_ELF_H_
#include <cstdint>
#include <kernel/task/task.h>

namespace objects::elf {
struct ElfObject {
    uint8_t                     type;
    uint64_t                    startAddr;
    uint64_t                    entryPoint;
    uint64_t                    relaAddr;
    std::vector<task::Mapping*> mappings;
    std::vector<ElfObject*>     dependencies;
    // Elf64_Addr                  symtabVirtual;
    // Elf64_Addr                  hashVirtual;
    Elf64_Addr  relaVirtual;
    Elf64_Xword relaSize;
    Elf64_Addr  baseAddr;
    ~ElfObject() {
        for (task::Mapping* mapping : this->mappings) {
            delete mapping;
        }
        for (ElfObject* obj : this->dependencies) {
            delete obj;
        }
    }
};
ElfObject* loadElfObject(int handle, size_t PHDRAddend);
}; // namespace objects::elf

#endif // _KERNEL_OBJECTS_ELF_H_
