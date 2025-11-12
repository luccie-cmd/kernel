#if !defined(_KERNEL_OBJECTS_ELF_H_)
#define _KERNEL_OBJECTS_ELF_H_
#include <cstdint>
#include <kernel/task/task.h>

namespace objects::elf {
struct ElfObject {
    uint8_t                     type;
    uint64_t                    startAddr;
    uint64_t                    entryPoint;
    std::vector<task::Mapping*> mappings;
    std::vector<ElfObject*>     dependencies;
    Elf64_Addr                  pltGotVirtual;
    uint64_t                    jmpSize;
    uint64_t                    jmpVirtual;
    Elf64_Addr                  symtabVirtual;
    Elf64_Addr                  hashVirtual;
    Elf64_Addr                  relaVirtual;
    Elf64_Xword                 relaSize;
    uint64_t                    strsize;
    Elf64_Addr                  baseAddr;
    uint32_t                    nSymbols;
    uint8_t*                    strtab;
    std::vector<Elf64_Rela*>    relaEntries;
    ~ElfObject() {
        for (task::Mapping* mapping : this->mappings) {
            delete mapping;
        }
        for (ElfObject* obj : this->dependencies) {
            delete obj;
        }
    }
};
ElfObject* loadElfObject(uint64_t handle, size_t PHDRAddend);
}; // namespace objects::elf

#endif // _KERNEL_OBJECTS_ELF_H_
