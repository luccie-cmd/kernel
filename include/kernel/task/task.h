#if !defined(_KERNEL_TASK_TASK_H_)
#define _KERNEL_TASK_TASK_H_
#include "types.h"

#include <common/io/regs.h>
#include <cstdint>
#include <kernel/mmu/vmm/types.h>

namespace task {
struct ProcessMemoryMapping {
    size_t                fileOffset;
    size_t                virtualStart;
    size_t                length;
    size_t                fileIdx;
    int                   permissions;
    ProcessMemoryMapping* next;
};
enum struct ThreadStatus {
    READY,
    RUNNING,
    BLOCKED,
};
struct Thread {
    pid_t          tid;
    io::Registers* registers;
    uint8_t        fpuState[512];
    ThreadStatus   status;
    Thread*        next;
};
struct Process {
    pid_t                 pid;
    bool                  hasStarted;
    mmu::vmm::PML4*       pml4;
    Process *             next, *prev;
    std::vector<Process*> children;
    Process*              parent;
    ProcessMemoryMapping* memoryMapping;
    Thread*               threads;
};
struct Mapping {
    uint64_t virtualStart;
    uint64_t fileOffset;
    uint64_t length;
    int      permissions;
};
void  initialize();
bool  isInitialized();
pid_t getNewPID();
//                                                                                          {{virtual
//                                                                                          start,
//                                                                                          file
//                                                                                          offset},
//                                                                                          length}
void makeNewProcess(pid_t pid, uint64_t entryPoint, size_t fileIdx, std::vector<Mapping*> mappings);
void attachThread(pid_t pid, uint64_t entryPoint);
void mapProcess(mmu::vmm::PML4* pml4, uint64_t virtualAddress);
void nextProc();
void printInfo();
}; // namespace task

#endif // _KERNEL_TASK_TASK_H_
