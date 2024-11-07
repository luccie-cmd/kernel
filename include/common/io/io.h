#if !defined(_COMMON_IO_IO_H_)
#define _COMMON_IO_IO_H_
#include <cstdint>
#define DBG_PORT 0xE9

namespace io{
    void outb(uint16_t port, uint8_t data);
    void cli();
    void sti();
    void invalpg(void* addr);
    void wcr3(uint64_t newCR3);
    uint64_t rcr3();
}

#endif // _COMMON_IO_IO_H_
