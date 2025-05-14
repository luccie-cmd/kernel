#include <common/dbg/dbg.h>
#include <kernel/task/task.h>
#define MODULE "Task manager"

namespace task {
bool     initialized = false;
Process* currentProc;
pid_t    pids = 0;
void     initialize() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    initialized          = true;
    currentProc          = new Process;
    currentProc->running = true;
    currentProc->pid     = KERNEL_PID;
    currentProc->next    = currentProc;
    pids++;
    dbg::popTrace();
}
bool isInitialized() {
    return initialized;
}
pid_t getCurrentPID() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    dbg::popTrace();
    return currentProc->pid;
}
pid_t getNewPID() {
    if (!isInitialized()) {
        initialize();
    }
    return pids++;
}
void makeNewProcess(pid_t pid) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    Process* proc = new Process;
    proc->next    = currentProc;
    proc->pid     = pid;
    proc->running = false;
    currentProc   = proc;
    dbg::popTrace();
}
void runProc(pid_t pid) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    Process* head     = currentProc;
    Process* saveHead = head;
    while (head) {
        if (head->pid == pid) {
            head->running = true;
            dbg::popTrace();
            return;
        }
        head = head->next;
        if (saveHead == head) {
            dbg::printm(MODULE, "No PID present with PID %llu\n", pid);
            std::abort();
        }
    }
    __builtin_unreachable();
}
}; // namespace task