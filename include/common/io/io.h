#if !defined(_COMMON_IO_IO_H_)
#define _COMMON_IO_IO_H_
#include <cstdint>
#define DBG_PORT 0xE9
#define ALIGNUP(data, align) ((data + align - 1) & ~(align - 1))
#define ALIGNDOWN(data, align) (data & ~(align - 1))

template<typename T>
inline void forceReadVolatile(const T& var) {
    volatile T tmp = *(volatile const T*)&var;
    asm volatile("" : "+m"(tmp));
}

namespace io{
    void outb(uint16_t port, uint8_t data);
    void outl(uint16_t port, uint32_t data);
    uint8_t inb(uint16_t port);
    uint16_t inw(uint16_t port);
    uint32_t inl(uint16_t port);
    void insl(uint16_t port, void *buffer, uint32_t count);
    uint64_t rdmsr(uint32_t msr);
    void wrmsr(uint32_t msr, uint64_t val);
    void cli();
    void sti();
    void invalpg(void* addr);
    void wcr3(uint64_t newCR3);
    uint64_t rcr3();
    uint64_t rcr2();
}

#endif // _COMMON_IO_IO_H_
