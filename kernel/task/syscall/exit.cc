#include <common/dbg/dbg.h>
#include <csignal>
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
        dbg::printf("TODO: Free process resources\n");
        dbg::printf("Process %u has exited (code %u)\n", currentProc->pid, exitCode);
        if (currentProc->parent) {
            if (currentProc->parent->waitingFor == currentProc->pid) {
                currentProc->parent->waitingFor = 0;
                currentProc->parent->waitStatus = exitCode;
                unblockProcess(currentProc->parent);
            }
            sendSignal(currentProc->parent, SIGCHLD);
        }
        currentProc->state    = ProcessState::Zombie;
        currentProc->exitCode = exitCode;
        zombieProcs.push(currentProc);
    } else {
        currentThread->status   = ThreadStatus::Dead;
        currentThread->exitCode = exitCode;
    }
    dbg::printf("TODO: Free thread resources\n");
    dbg::popTrace();
    return 0;
}
} // namespace syscall
}; // namespace task