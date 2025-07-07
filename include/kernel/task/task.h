#if !defined(_KERNEL_TASK_TASK_H_)
#define _KERNEL_TASK_TASK_H_
#include "types.h"

#include <common/io/regs.h>
#include <cstdint>
#include <functional>
#include <kernel/mmu/vmm/types.h>
#include <vector>
#define SIGABORT 1

namespace task {
struct ProcessMemoryMapping {
    size_t                fileOffset;
    size_t                virtualStart;
    size_t                alignment;
    size_t                fileLength;
    size_t                memLength;
    size_t                fileIdx;
    int                   permissions;
    ProcessMemoryMapping* next;
};
enum struct ThreadStatus {
    Ready,
    Running,
    Blocked,
    Zombie,
    Dead,
};
struct Thread {
    pid_t          tid;
    io::Registers* registers;
    uint64_t       fsBase;
    uint8_t        fpuState[512];
    uint8_t        exitCode;
    ThreadStatus   status;
    Thread*        next;
};
enum struct ProcessState {
    Ready,
    Zombie,
    Blocked,
    Running,
};
using signalHandler = std::function<void(size_t)>;
struct Process {
    pid_t                                     pid;
    pid_t                                     waitingFor;
    ProcessState                              state;
    uint8_t                                   waitStatus;
    uint8_t                                   exitCode;
    bool                                      hasStarted;
    mmu::vmm::PML4*                           pml4;
    Process *                                 next, *prev;
    std::vector<Process*>                     children;
    Process*                                  parent;
    ProcessMemoryMapping*                     memoryMapping;
    Thread*                                   threads;
    ProcessMemoryMapping*                     dynamicMapping;
    std::unordered_map<size_t, signalHandler> signals;
};
struct Mapping {
    uint64_t virtualStart;
    uint64_t fileOffset;
    uint64_t memLength;
    uint64_t fileLength;
    uint64_t alignment;
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
void makeNewProcess(pid_t pid, uint64_t entryPoint, size_t fileIdx, std::vector<Mapping*> mappings,
                    Mapping* dynamicMapping);
void attachThread(pid_t pid, uint64_t entryPoint);
void mapProcess(mmu::vmm::PML4* pml4, uint64_t virtualAddress);
void nextProc();
Process* getCurrentProc();
Thread*  getCurrentThread();
void     unblockProcess(Process* proc);
void     blockProcess(Process* proc);
void     sendSignal(Process* proc, size_t signal);
void     printInfo();
}; // namespace task

#endif // _KERNEL_TASK_TASK_H_
