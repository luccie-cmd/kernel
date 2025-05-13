#if !defined(_KERNEL_OBJECTS_ELF_H_)
#define _KERNEL_OBJECTS_ELF_H_
#include <cstdint>

namespace objects::elf {
struct ElfObject {};
ElfObject* loadElfObject(uint8_t* buffer, uint64_t length);
}; // namespace objects::elf

#endif // _KERNEL_OBJECTS_ELF_H_
