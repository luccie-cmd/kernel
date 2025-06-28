#if !defined(_KERNEL_OBJECTS_ELF_H_)
#define _KERNEL_OBJECTS_ELF_H_
#include <cstdint>
#include <kernel/task/task.h>

namespace objects::elf {
struct ElfObject {
    uint8_t                     type;
    uint64_t                    startAddr;
    uint64_t                    entryPoint;
    task::Mapping*              dynamicSection;
    uint64_t                    relaAddr;
    std::vector<task::Mapping*> mappings;
    std::vector<ElfObject*>     dependencies;
};
ElfObject* loadElfObject(int handle);
}; // namespace objects::elf

#endif // _KERNEL_OBJECTS_ELF_H_
