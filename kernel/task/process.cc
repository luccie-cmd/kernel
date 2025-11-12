#include <algorithm>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/spinlock.h>
#include <cstdlib>
#include <cstring>
#include <elf.h>
#include <kernel/hal/arch/x64/irq/irq.h>
#include <kernel/mmu/mmu.h>
#include <kernel/objects/elf.h>
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
static uint32_t getSymbolCount(Process* proc, uint64_t symtabVirtual) {
    ProcessMemoryMapping* mapping = findMappingInProcess(proc, symtabVirtual);
    if (!mapping) return 0;
    uint64_t symtabSizeInMapping = mapping->memLength - (symtabVirtual - mapping->virtualStart);
    return symtabSizeInMapping / sizeof(Elf64_Sym);
}
static Elf64_Sym* loadSymByName(Process* proc, const char* name, uint64_t* outBaseAddr) {
    Elf64_Sym* sym = new Elf64_Sym;
    for (size_t j = 0; j < proc->elfObj->dependencies.size() + 1; ++j) {
        objects::elf::ElfObject* loadObj       = j == proc->elfObj->dependencies.size()
                                                     ? proc->elfObj
                                                     : proc->elfObj->dependencies.at(j);
        uint64_t                 neededVirtual = loadObj->symtabVirtual;
        size_t                   numSyms       = getSymbolCount(proc, loadObj->symtabVirtual);
        *outBaseAddr                           = loadObj->baseAddr;
        ProcessMemoryMapping* mapping          = findMappingInProcess(proc, loadObj->symtabVirtual);
        for (size_t i = 0; i < numSyms; ++i) {
            vfs::seek(mapping->fileIdx,
                      (neededVirtual - *outBaseAddr) + mapping->fileOffset + i * sizeof(Elf64_Sym));
            vfs::readFile(mapping->fileIdx, sizeof(Elf64_Sym), sym);
            if (sym->st_value == 0) {
                continue;
            }
            const char* symName = (const char*)loadObj->strtab + sym->st_name;
            if (std::strcmp(name, symName) == 0) {
                return sym;
            }
        }
    }
    dbg::printm(MODULE, "ERROR: No symbol named %s found\n", name);
    std::abort();
}
static uint64_t resolveAddr(Process* proc, uint32_t symIdx, Elf64_Rela* rela) {
    uint64_t   baseAddr = 0;
    Elf64_Sym* sym      = nullptr;
    for (size_t j = 0; j < proc->elfObj->dependencies.size() + 1; ++j) {
        objects::elf::ElfObject* loadObj = j == proc->elfObj->dependencies.size()
                                               ? proc->elfObj
                                               : proc->elfObj->dependencies.at(j);
        if (std::find(loadObj->relaEntries.begin(), loadObj->relaEntries.end(), rela) !=
            loadObj->relaEntries.end()) {
            uint64_t neededVirtual        = loadObj->symtabVirtual;
            baseAddr                      = loadObj->baseAddr;
            ProcessMemoryMapping* mapping = findMappingInProcess(proc, neededVirtual);
            if (!mapping) {
                dbg::printm(MODULE, "Failed to find the symbol table entry\n");
                std::abort();
            }
            sym = new Elf64_Sym;
            vfs::seek(mapping->fileIdx, (neededVirtual - baseAddr) + mapping->fileOffset +
                                            symIdx * sizeof(Elf64_Sym));
            vfs::readFile(mapping->fileIdx, sizeof(Elf64_Sym), sym);
            Elf64_Sym nullEntry = (Elf64_Sym){0, 0, 0, 0, 0, 0};
            if (std::memcmp(&nullEntry, sym, sizeof(Elf64_Sym)) == 0) {
                delete sym;
                sym = nullptr;
                continue;
            }
            if (sym->st_value == 0) {
                sym = loadSymByName(proc, (const char*)loadObj->strtab + sym->st_name, &baseAddr);
            }
#ifdef DEBUG
            dbg::printm(MODULE, "Found symbolidx %lu (name %s)\n", symIdx,
                        loadObj->strtab + sym->st_name);
#endif
            break;
        }
    }
    if (sym == nullptr) {
        dbg::printm(MODULE, "Couldn't find needed RELA entry\n");
        std::abort();
    }
    Elf64_Sym nullEntry = (Elf64_Sym){0, 0, 0, 0, 0, 0};
    if (std::memcmp(&nullEntry, sym, sizeof(Elf64_Sym)) == 0) {
        dbg::printm(MODULE, "Unable to find the needed symbol\n");
        std::abort();
    }
#ifdef DEBUG
    dbg::printm(MODULE, "sym->st_value = %lx, baseaddr = %lx\n", sym->st_value, baseAddr);
#endif
    uint64_t ret = sym->st_value + baseAddr;
    delete sym;
    return ret;
}
static void applyRela(Process* proc, Elf64_Rela* rela, uint64_t bufferAddr) {
#ifdef DEBUG
    dbg::printm(MODULE, "Applying at %lx symidx %u type %u addend %lx\n", rela->r_offset,
                ELF64_R_SYM(rela->r_info), ELF64_R_TYPE(rela->r_info), rela->r_addend);
#endif
    switch (ELF64_R_TYPE(rela->r_info)) {
    case R_X86_64_RELATIVE: {
        *(uint64_t*)(bufferAddr + (rela->r_offset & 0xFFF)) = proc->baseAddr + rela->r_addend;
    } break;
    case R_X86_64_GLOB_DAT:
    case R_X86_64_JUMP_SLOT: {
        uint64_t resolvedAddr = resolveAddr(proc, ELF64_R_SYM(rela->r_info), rela);
        *(uint64_t*)(bufferAddr + (rela->r_offset & 0xFFF)) = resolvedAddr;
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
void makeNewProcess(pid_t pid, objects::elf::ElfObject* obj) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    Process* proc = new Process;
    proc->pid     = pid;
    proc->state   = ProcessState::Ready;
    proc->pml4    = mmu::vmm::getPML4(pid);
    proc->signals.insert({SIGKILL, (signalHandler)defaultSignalHandler});
    if (!obj->relaVirtual) {
        dbg::printm(MODULE, "WARNING: No RELA address was passed in\n");
    }
    proc->baseAddr = obj->startAddr;
    proc->threads  = nullptr;
    for (size_t j = 0; j < obj->dependencies.size() + 1; ++j) {
        objects::elf::ElfObject* loadObj =
            j == obj->dependencies.size() ? obj : obj->dependencies.at(j);
        for (Mapping* mapping : loadObj->mappings) {
            ProcessMemoryMapping* memMapping = new ProcessMemoryMapping;
            memMapping->fileIdx              = mapping->handle;
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
                ProcessMemoryMapping* procMapping = findMappingInProcess(
                    proc, ALIGNDOWN(mapping->virtualStart, mapping->alignment));
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
            dbg::printm(
                MODULE,
                "Added new mapping to process %llu (VADDR = 0x%llx Mapped Length = 0x%llx)\n",
                proc->pid, memMapping->virtualStart, memMapping->memLength);
#endif
            memMapping->next    = proc->memoryMapping;
            proc->memoryMapping = memMapping;
        }
        if (loadObj->relaVirtual) {
            ProcessMemoryMapping* mapping = findMappingInProcess(proc, loadObj->relaVirtual);
            if (mapping == nullptr) {
                dbg::printm(MODULE, "Failed to find process memory mapping for rela virtual\n");
#ifdef DEBUG
                dbg::printf("Tried loading loadObj->relaVirtual = %lx\n", loadObj->relaVirtual);
#endif
                std::abort();
            }
            for (size_t i = 0; i < loadObj->relaSize; i += sizeof(Elf64_Rela)) {
                Elf64_Rela* rela = new Elf64_Rela;
                vfs::seek(mapping->fileIdx,
                          (loadObj->relaVirtual - loadObj->baseAddr) + mapping->fileOffset + i);
                vfs::readFile(mapping->fileIdx, sizeof(Elf64_Rela), rela);
                rela->r_offset += loadObj->baseAddr;
                if (rela->r_info == 0) break;
#ifdef DEBUG
                dbg::printm(MODULE,
                            "Loaded rela entry (from rela) at %lx symidx %u type %u addend %lx\n",
                            rela->r_offset, ELF64_R_SYM(rela->r_info), ELF64_R_TYPE(rela->r_info),
                            rela->r_addend);
#endif
                loadObj->relaEntries.push_back(rela);
                proc->relas.push_back(rela);
            }
        }
        if (loadObj->jmpVirtual) {
            ProcessMemoryMapping* mapping = findMappingInProcess(proc, loadObj->jmpVirtual);
            if (mapping == nullptr) {
                dbg::printm(MODULE, "Failed to find process memory mapping for rela virtual\n");
#ifdef DEBUG
                dbg::printf("Tried loading loadObj->jmpVirtual = %lx\n", loadObj->jmpVirtual);
#endif
                std::abort();
            }
            for (size_t i = 0; i < loadObj->jmpSize; i += sizeof(Elf64_Rela)) {
                Elf64_Rela* rela = new Elf64_Rela;
                vfs::seek(mapping->fileIdx,
                          (loadObj->jmpVirtual - loadObj->baseAddr) + mapping->fileOffset + i);
                vfs::readFile(mapping->fileIdx, sizeof(Elf64_Rela), rela);
                rela->r_offset += loadObj->baseAddr;
                if (rela->r_info == 0) break;
#ifdef DEBUG
                dbg::printm(MODULE,
                            "Loaded rela entry (from jmp) at %lx symidx %u type %u addend %lx\n",
                            rela->r_offset, ELF64_R_SYM(rela->r_info), ELF64_R_TYPE(rela->r_info),
                            rela->r_addend);
#endif
                loadObj->relaEntries.push_back(rela);
                proc->relas.push_back(rela);
            }
        }
    }
#ifdef DEBUG
    dbg::printm(MODULE, "Added %llu rela entries\n", proc->relas.size());
#endif
    proc->elfObj = obj;
    // std::abort();
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
    attachThread(pid, obj->entryPoint);
    dbg::popTrace();
}
}; // namespace task