#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/spinlock.h>
#include <cstdlib>
#include <cstring>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <kernel/mmu/mmu.h>
#include <kernel/task/task.h>
#include <kernel/vfs/vfs.h>
#define MODULE "Program loader"

namespace task {
extern Process*      globalParent;
extern Process*      currentProc;
extern std::Spinlock currentProcLock;
// extern std::Spinlock currentThreadLock;
extern std::vector<GSbase*> cpuLocals;
static Process*             findProcByPml4(mmu::vmm::PML4* pml4) {
    currentProcLock.lock();
    if (currentProc->pml4 == pml4) {
        currentProcLock.unlock();
        return currentProc;
    }
    Process* head = currentProc;
    while (head->pml4 != pml4) {
        head = head->next;
        if (head == currentProc) {
            dbg::printm(MODULE, "Circular dependency exceeded, no process found for PML4 0x%llx\n",
                                    pml4);
            std::abort();
        }
    }
    currentProcLock.unlock();
    return head;
}
static inline bool isInRange(uint64_t start, uint64_t end, uint64_t address) {
    return (address >= start && address < end);
}
static ProcessMemoryMapping* findMappingInProcess(Process* proc, uint64_t virtualAddress) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    ProcessMemoryMapping* head = proc->memoryMapping;
    while (head) {
        if (isInRange(head->virtualStart, head->virtualStart + head->memLength, virtualAddress)) {
            dbg::popTrace();
            return head;
        }
        head = head->next;
    }
    dbg::popTrace();
    return nullptr;
}
static std::vector<Elf64_Rela*> findRelasInPage(Process* proc, uint64_t virtualAddr) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    std::vector<Elf64_Rela*> newRelas;
    for (Elf64_Rela* rela : proc->relas) {
        if (isInRange(virtualAddr, virtualAddr + PAGE_SIZE, rela->r_offset)) {
            newRelas.push_back(rela);
        }
    }
    dbg::popTrace();
    return newRelas;
}
void __attribute__((section(".trampoline.text"))) defaultSignalHandler(size_t signal) {
    switch (signal) {
    case SIGKILL: {
        cleanProc(getCurrentProc()->pid, signal);
        nextProc();
        __builtin_unreachable();
    } break;
    default: {
        dbg::printm(MODULE, "TODO: Handle signal %lu\n", signal);
        std::abort();
    }
    }
}
static void applyRela(Process* proc, Elf64_Rela* rela, uint64_t bufferAddr) {
    switch (ELF64_R_TYPE(rela->r_info)) {
    case R_X86_64_RELATIVE: {
        *(uint64_t*)(bufferAddr + (rela->r_offset & 0xFFF)) = proc->baseAddr + rela->r_addend;
    } break;
    default: {
        dbg::printm(MODULE, "TODO: Handle rela 0x%llx type: %llu sym: %llu addend: 0x%llx\n",
                    rela->r_offset, ELF64_R_TYPE(rela->r_info), ELF64_R_SYM(rela->r_info),
                    rela->r_addend);
        std::abort();
    } break;
    }
}
std::Spinlock mapProcLock;
void          mapProcess(mmu::vmm::PML4* pml4, uint64_t virtualAddress) {
    mapProcLock.lock();
    dbg::addTrace(__PRETTY_FUNCTION__);
    Process* proc = findProcByPml4(pml4);
    proc->state   = ProcessState::Blocked;
    cpuLocals.at(hal::arch::x64::irq::getAPICID())->currentThread->status = ThreadStatus::Blocked;
    ProcessMemoryMapping* mapping = findMappingInProcess(proc, virtualAddress);
    if (mapping == nullptr) {
        std::abort();
    }
    if (mapping->fileIdx == static_cast<size_t>(-1)) {
        dbg::printm(MODULE, "TODO: Map 0x%llx as data\n", virtualAddress);
        std::abort();
    } else {
        uint8_t* bufferAddr = (uint8_t*)mmu::pmm::allocate();
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)bufferAddr, (uint64_t)bufferAddr,
                                   PROTECTION_KERNEL | PROTECTION_NOEXEC | PROTECTION_RW, MAP_PRESENT);
        size_t    remaining   = PAGE_SIZE;
        uintptr_t base        = ALIGNDOWN(virtualAddress, PAGE_SIZE);
        size_t    i           = 0;
        int       permissions = 0;
        while (remaining > 0) {
            mapping = findMappingInProcess(proc, base + i);
            if (mapping == nullptr) {
                bufferAddr[i++] = 0;
                remaining--;
                continue;
            }
            if (mapping->fileIdx == static_cast<size_t>(-1)) {
                dbg::printm(MODULE, "TODO: Map 0x%llx as data\n", base + i);
                std::abort();
            }
            uint64_t savedOffset = vfs::getOffset(mapping->fileIdx);
            size_t   readSize    = std::min(mapping->fileLength, remaining);
            vfs::seek(mapping->fileIdx, mapping->fileOffset + (base + i - mapping->virtualStart));
            vfs::readFile(mapping->fileIdx, readSize, (void*)(bufferAddr + i));
            vfs::seek(mapping->fileIdx, savedOffset);
            permissions |= mapping->permissions;
            remaining -= readSize;
            i += readSize;
        }
        if ((permissions & PROTECTION_RW) == 0) {
            dbg::printm(MODULE,
                                 "ERROR: No read or write permissions set for virtual address 0x%llu in PID "
                                          "%llu. TOOD: sysExit\n",
                                 virtualAddress, proc->pid);
            std::abort();
        }
        std::vector<Elf64_Rela*> toHandleRelas = findRelasInPage(proc, base);
        for (Elf64_Rela* rela : toHandleRelas) {
            applyRela(proc, rela, (uint64_t)bufferAddr);
        }
        uint64_t phys = mmu::pmm::allocate();
        mmu::vmm::mapPage(mmu::vmm::getPML4(KERNEL_PID), phys, base,
                                   PROTECTION_KERNEL | permissions, MAP_PRESENT);
        mmu::vmm::mapPage(proc->pml4, phys, base, permissions, MAP_PRESENT);
        std::memcpy((void*)base, bufferAddr, PAGE_SIZE);
        mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), base);
        mmu::vmm::unmapPage(mmu::vmm::getPML4(KERNEL_PID), (uint64_t)bufferAddr);
        mmu::pmm::free((uint64_t)bufferAddr, PAGE_SIZE);
    }
    cpuLocals.at(hal::arch::x64::irq::getAPICID())->currentThread->status = ThreadStatus::Ready;
    proc->state = ProcessState::Ready;
    mapProcLock.unlock();
    dbg::popTrace();
}
void makeNewProcess(pid_t pid, uint64_t entryPoint, size_t fileIdx, Elf64_Addr baseAddr,
                    std::vector<Mapping*> mappings, Elf64_Addr relaVirtual, Elf64_Xword relaSize) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    Process* proc = new Process;
    proc->pid     = pid;
    proc->state   = ProcessState::Ready;
    proc->pml4    = mmu::vmm::getPML4(pid);
    proc->signals.insert({SIGKILL, (signalHandler)defaultSignalHandler});
    if (!relaVirtual) {
        dbg::printm(MODULE, "WARNING: No RELA address was passed in\n");
    }
    proc->baseAddr = baseAddr;
    proc->threads  = nullptr;
    for (Mapping* mapping : mappings) {
        ProcessMemoryMapping* memMapping = new ProcessMemoryMapping;
        memMapping->fileIdx              = fileIdx;
        memMapping->fileOffset           = mapping->fileOffset;
        memMapping->memLength            = mapping->memLength;
        memMapping->fileLength           = mapping->fileLength;
        memMapping->permissions          = mapping->permissions;
        memMapping->virtualStart         = mapping->virtualStart;
        memMapping->alignment            = mapping->alignment;
        size_t    offset                 = memMapping->virtualStart % PAGE_SIZE;
        size_t    totalLen               = ALIGNUP(memMapping->memLength + offset, PAGE_SIZE);
        uintptr_t mapBase                = ALIGNDOWN(memMapping->virtualStart, PAGE_SIZE);
        if (memMapping->virtualStart % PAGE_SIZE == 0) {
            for (size_t i = 0; i < totalLen; i += PAGE_SIZE) {
                mmu::vmm::mapPage(mmu::vmm::getPML4(pid), ONDEMAND_MAP_ADDRESS, mapBase + i,
                                  memMapping->permissions, 0);
            }
        } else {
            ProcessMemoryMapping* procMapping =
                findMappingInProcess(proc, ALIGNDOWN(mapping->virtualStart, mapping->alignment));
            if (procMapping == nullptr) {
                for (size_t i = 0; i < totalLen; i += PAGE_SIZE) {
                    mmu::vmm::mapPage(mmu::vmm::getPML4(pid), ONDEMAND_MAP_ADDRESS, mapBase + i,
                                      memMapping->permissions, 0);
                }
            } else {
                dbg::printm(MODULE, "TODO: Further memory permisssion sharing\n");
                std::abort();
            }
        }
#ifdef DEBUG
        dbg::printm(MODULE,
                    "Added new mapping to process %llu (VADDR = 0x%llx Mapped Length = 0x%llx)\n",
                    proc->pid, memMapping->virtualStart, memMapping->memLength);
#endif
        memMapping->next    = proc->memoryMapping;
        proc->memoryMapping = memMapping;
    }
    if (relaVirtual) {
        ProcessMemoryMapping* mapping = findMappingInProcess(proc, relaVirtual);
        for (size_t i = 0; i < relaSize; i += sizeof(Elf64_Rela)) {
            Elf64_Rela* rela = new Elf64_Rela;
            vfs::seek(mapping->fileIdx, (relaVirtual - proc->baseAddr) + mapping->fileOffset +
                                            i * sizeof(Elf64_Rela));
            vfs::readFile(mapping->fileIdx, sizeof(Elf64_Rela), rela);
            rela->r_offset += proc->baseAddr;
            proc->relas.push_back(rela);
        }
        dbg::printm(MODULE, "Added %llu rela entries\n", proc->relas.size());
    }
    if (globalParent == nullptr) {
        globalParent = proc;
    }
    proc->parent = currentProc;
    if (currentProc == nullptr) {
        currentProc = proc;
        proc->next  = proc;
        proc->prev  = proc;
    } else {
        currentProc->children.push_back(proc);
        proc->prev              = currentProc->prev;
        proc->next              = currentProc;
        currentProc->prev->next = proc;
        currentProc->prev       = proc;
    }
    attachThread(pid, entryPoint);
    dbg::popTrace();
}
}; // namespace task