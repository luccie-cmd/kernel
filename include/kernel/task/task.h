#if !defined(_KERNEL_TASK_TASK_H_)
#define _KERNEL_TASK_TASK_H_
#include "types.h"
#include <cstdint>
#include <common/io/regs.h>

namespace task {
struct Process {
    pid_t    pid;
    io::Registers *registers;
    bool hasStarted;
    Process* next;
    uint8_t fpuState[512];
};
void  initialize();
bool  isInitialized();
pid_t getCurrentPID();
pid_t getNewPID();
void  makeNewProcess(pid_t pid, uint8_t* codeBuffer, uint64_t codeSize, uint64_t entryPoint);
void  nextProc();
}; // namespace task

#endif // _KERNEL_TASK_TASK_H_
