#if !defined(_KERNEL_TASK_TASK_H_)
#define _KERNEL_TASK_TASK_H_
#include "types.h"

namespace task {
struct Process {
    bool     running;
    pid_t    pid;
    Process* next;
};
void  initialize();
bool  isInitialized();
pid_t getCurrentPID();
pid_t getNewPID();
void  makeNewProcess(pid_t pid);
void  runProc(pid_t pid);
}; // namespace task

#endif // _KERNEL_TASK_TASK_H_
