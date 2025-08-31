#include <common/dbg/dbg.h>
#include <kernel/task/syscall.h>
#include <kernel/task/task.h>
#include <queue>

namespace task {
extern std::queue<Process*> zombieProcs;
namespace syscall {
size_t sysExit(SyscallRegs* regs) {
    // TODO: Sanity checks
    dbg::addTrace(__PRETTY_FUNCTION__);
    Process* currentProc   = getCurrentProc();
    Thread*  currentThread = getCurrentThread();
    uint8_t  exitCode      = regs->rdi;
    // Last thread
    if (currentThread->next == currentThread && currentProc->threads == currentThread) {
        cleanProc(currentProc->pid, exitCode);
    } else {
        cleanThread(currentProc->pid, currentThread->tid, exitCode);
    }
    dbg::popTrace();
    return 0;
}
} // namespace syscall
}; // namespace task