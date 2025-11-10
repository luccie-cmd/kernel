#if !defined(_KERNEL_TASK_TASK_H_)
#define _KERNEL_TASK_TASK_H_
#include "types.h"

#include <common/io/regs.h>
#include <cstdint>
#include <elf.h>
#include <functional>
#include <kernel/mmu/vmm/types.h>
#include <vector>
#define SIGABORT 1
#define SIGKILL  2
#define SIGCHILD 3

namespace task {
struct ProcessMemoryMapping {
    size_t                fileOffset;
    size_t                virtualStart;
    size_t                alignment;
    size_t                fileLength;
    size_t                memLength;
    size_t                fileIdx;
    uint8_t               permissions;
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
    std::vector<Elf64_Rela*>                  relas;
    Elf64_Addr                                baseAddr;
    std::unordered_map<size_t, signalHandler> signals;
};
struct Mapping {
    uint64_t virtualStart;
    uint64_t fileOffset;
    uint64_t memLength;
    uint64_t fileLength;
    uint64_t alignment;
    uint8_t  permissions;
};
struct GSbase {
    Thread*  currentThread; // 0x00
    Process* currentProc;   // 0x08
    uint64_t kernelCR3;     // 0x10
    uint64_t stackTop;      // 0x18
} __attribute__((packed));
void  initialize();
bool  isInitialized();
pid_t getNewPID();
//                                                                                          {{virtual
//                                                                                          start,
//                                                                                          file
//                                                                                          offset},
//                                                                                          length}
void     makeNewProcess(pid_t pid, uint64_t entryPoint, size_t fileIdx, Elf64_Addr baseAddr,
                        std::vector<Mapping*> mappings, Elf64_Addr relaVirtual, Elf64_Xword relaSize);
void     attachThread(pid_t pid, uint64_t entryPoint);
void     mapProcess(mmu::vmm::PML4* pml4, uint64_t virtualAddress);
void     nextProc();
void     cleanProc(pid_t pid, uint8_t exitCode);
void     cleanThread(pid_t pid, pid_t tid, uint8_t exitCode);
Process* getCurrentProc();
Thread*  getCurrentThread();
void     unblockProcess(Process* proc);
void     blockProcess(Process* proc);
void     sendSignal(Process* proc, size_t signal);
void     printInfo();
}; // namespace task

#endif // _KERNEL_TASK_TASK_H_
