#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <cstring>
#include <kernel/mmu/mmu.h>
#include <kernel/task/syscall.h>
#include <kernel/task/task.h>
#include <kernel/vfs/vfs.h>
#include <unistd.h>

namespace task::syscall {
size_t sysWrite(SyscallRegs* regs) {
    // TODO: Sanity checks
    dbg::addTrace(__PRETTY_FUNCTION__);
    Process* currentProc   = getCurrentProc();
    Thread*  currentThread = getCurrentThread();
    size_t   fd            = regs->rdi;
    uint64_t userBuffer    = regs->rsi;
    size_t   length        = regs->rdx;
    char*    buffer        = new char[length + 1];
    for (size_t i = 0; i < ALIGNUP(length, PAGE_SIZE); i += PAGE_SIZE, userBuffer += PAGE_SIZE) {
        uint64_t physicalAddr =
            mmu::vmm::getPhysicalAddr(currentProc->pml4, userBuffer, true, true);
        if (physicalAddr == 0) {
            dbg::printf("Segfault, physical address was 0 and not mapped\n");
            std::abort();
        }
        if (physicalAddr == ONDEMAND_MAP_ADDRESS) {
            mapProcess(currentProc->pml4, userBuffer);
            physicalAddr = mmu::vmm::getPhysicalAddr(currentProc->pml4, userBuffer, false, false);
        }
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), physicalAddr, userBuffer,
                          PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
        std::memcpy(buffer, (void*)userBuffer, (std::min(regs->rdx + 1, (uint64_t)PAGE_SIZE)));
        if ((int64_t)regs->rdx - PAGE_SIZE < 0) {
            regs->rdx = 0;
        } else {
            regs->rdx -= PAGE_SIZE;
        }
        mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), userBuffer);
    }
    if (fd == STDOUT_FILENO) {
        dbg::printf("%.*s", length, buffer);
    } else {
        vfs::writeFile(fd, length, buffer);
    }
    currentThread->status = ThreadStatus::Ready;
    dbg::popTrace();
    return length;
}
}; // namespace task::syscall