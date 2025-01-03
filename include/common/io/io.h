#if !defined(_COMMON_IO_IO_H_)
#define _COMMON_IO_IO_H_
#include <cstdint>
#define DBG_PORT 0xE9

namespace io{
    void outb(uint16_t port, uint8_t data);
    void outl(uint16_t port, uint32_t data);
    uint8_t inb(uint16_t port);
    uint16_t inw(uint16_t port);
    uint32_t inl(uint16_t port);
    void insl(uint16_t port, void *buffer, uint32_t count);
    void cli();
    void sti();
    void invalpg(void* addr);
    void wcr3(uint64_t newCR3);
    uint64_t rcr3();
    uint64_t rcr2();
}

#endif // _COMMON_IO_IO_H_
