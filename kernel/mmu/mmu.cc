#include <kernel/mmu/mmu.h>

namespace mmu
{
void printInfo()
{
    pmm::printInfo();
    heap::printInfo();
}
} // namespace mmu