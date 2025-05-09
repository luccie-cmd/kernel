#if !defined(_KERNEL_HAL_ARCH_X64_IRQ_IRQ_H_)
#define _KERNEL_HAL_ARCH_X64_IRQ_IRQ_H_
#include <functional>
#include <common/io/regs.h>

namespace hal::arch::x64::irq
{
void init();
// Overrides an IRQ that should always be on the same vector (PIT, PS/2, NIC)
void overrideIrq(uint32_t IRQ, std::function<void(io::Registers*)> func);
// Requests an  IRQ and returns its vector
uint8_t requestIrq(std::function<void(io::Registers*)> func);
};

#endif // _KERNEL_HAL_ARCH_X64_IRQ_IRQ_H_
