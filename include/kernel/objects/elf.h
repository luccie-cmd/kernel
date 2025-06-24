#if !defined(_KERNEL_OBJECTS_ELF_H_)
#define _KERNEL_OBJECTS_ELF_H_
#include <cstdint>
#include <kernel/task/task.h>

namespace objects::elf {
struct ElfObject {
    uint8_t type;
    uint64_t startAddr;
    uint64_t entryPoint;
    uint64_t dynamicSection;
    uint64_t gotAddr;
    uint64_t pltAddr;
    uint64_t dynsymAddr;
    uint64_t dynstrAddr;
    uint64_t relaAddr;
    std::vector<task::Mapping*> mappings;
    std::vector<ElfObject*> dependencies;
};
ElfObject* loadElfObject(int handle);
}; // namespace objects::elf

#endif // _KERNEL_OBJECTS_ELF_H_
