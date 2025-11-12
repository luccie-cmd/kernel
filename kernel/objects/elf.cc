#include <cassert>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstring>
#include <elf.h>
#include <kernel/mmu/mmu.h>
#include <kernel/mmu/vmm/vmm.h>
#include <kernel/objects/elf.h>
#include <kernel/vfs/vfs.h>
#include <queue>
#include <string>
#define MODULE "ELF Loader"

namespace objects::elf {
static bool verifyHeader(Elf64_Ehdr* hdr) {
    if (hdr->e_ident[EI_MAG0] != ELFMAG0 || hdr->e_ident[EI_MAG1] != ELFMAG1 ||
        hdr->e_ident[EI_MAG2] != ELFMAG2 || hdr->e_ident[EI_MAG3] != ELFMAG3) {
        return false;
    }
    if (hdr->e_type != ET_DYN) {
        dbg::printm(MODULE, "No support for ET_DYN files\n");
        return false;
    }
    return true;
}
static const char* makeStringFromStrTab(uint8_t* strtab, uint64_t size, uint64_t index) {
    std::vector<char> buffer;
    for (size_t i = index; i < size; ++i) {
        buffer.push_back(strtab[i]);
        if (strtab[i] == 0) {
            break;
        }
    }
    char* retBuffer = new char[buffer.size()];
    std::memcpy(retBuffer, buffer.data(), buffer.size());
    return retBuffer;
}
std::string prefix = "/tmpboot/lib/";
void        setPrefix(const char* path) {
    prefix = std::string(path);
}
ElfObject* loadElfObject(uint64_t handle, size_t PHDRAddend) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    char bitNess;
    vfs::seek(handle, 4);
    vfs::readFile(handle, 1, &bitNess);
    if (bitNess != ELFCLASS64) {
        dbg::printm(MODULE, "TODO: Add support for 32 bits\n");
        std::abort();
    }
    Elf64_Ehdr* header = new Elf64_Ehdr;
    vfs::seek(handle, 0);
    vfs::readFile(handle, sizeof(Elf64_Ehdr), header);
    if (!verifyHeader(header)) {
        dbg::printm(MODULE, "ELF Header couldn't be verified\n");
        std::abort();
    }
    if (header->e_type == ET_DYN && PHDRAddend == 0) {
        delete header;
        dbg::popTrace();
        return loadElfObject(handle, 0x1000);
    }
    ElfObject* obj     = new ElfObject;
    obj->type          = header->e_type;
    obj->entryPoint    = header->e_entry + PHDRAddend;
    obj->baseAddr      = PHDRAddend;
    Elf64_Off phOffset = header->e_phoff;
    vfs::seek(handle, phOffset);
    Elf64_Half  phentsize = header->e_phentsize;
    Elf64_Half  phnum     = header->e_phnum;
    Elf64_Phdr* dynPhdr   = nullptr;
    for (Elf64_Half i = 0; i < phnum; i++) {
        Elf64_Phdr* phdr = new Elf64_Phdr;
        vfs::readFile(handle, phentsize, phdr);
#ifdef DEBUG
        dbg::printm(MODULE,
                    "PHDR[%u]: Type: 0x%x, VAddr: 0x%lx, PAddr: 0x%lx, FileSz: 0x%lx, MemSz: "
                    "0x%lx, Flags: 0x%x, Align: 0x%lx, Offset: 0x%lx\n",
                    i, phdr->p_type, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz,
                    phdr->p_flags, phdr->p_align, phdr->p_offset);
#endif
        if (phdr->p_type != PT_LOAD && phdr->p_type != PT_DYNAMIC) {
            delete phdr;
            continue;
        }
        if (phdr->p_type == PT_LOAD) {
            task::Mapping* mapping = new task::Mapping;
            mapping->fileLength    = phdr->p_filesz;
            mapping->fileOffset    = phdr->p_offset;
            mapping->memLength     = phdr->p_memsz;
            mapping->permissions   = 0;
            if ((phdr->p_flags & PF_X) == 0) {
                mapping->permissions |= PROTECTION_NOEXEC;
            }
            if ((phdr->p_flags & PF_R) != 0 || (phdr->p_flags & PF_W) != 0) {
                mapping->permissions |= PROTECTION_RW;
            }
            mapping->virtualStart = phdr->p_vaddr + PHDRAddend;
            mapping->alignment    = phdr->p_align;
            mapping->handle       = handle;
            obj->mappings.push_back(mapping);
            delete phdr;
        } else if (phdr->p_type == PT_DYNAMIC) {
            if (dynPhdr != nullptr) {
                dbg::printm(MODULE, "ERROR: PT_DYNAMIC defined twice\n");
                std::abort();
            }
            dynPhdr = phdr;
        } else {
            dbg::printm(MODULE, "TODO: Handle phdr->p_type 0x%llx\n", phdr->p_type);
            std::abort();
        }
    }
    if (!dynPhdr) {
        dbg::printm(MODULE, "No dynamic program header found\n");
        delete header;
        dbg::popTrace();
        return obj;
    }
    vfs::seek(handle, dynPhdr->p_offset);
    size_t                  entryCount = dynPhdr->p_filesz / sizeof(Elf64_Dyn);
    std::queue<Elf64_Xword> dtNeededEntries;
    Elf64_Addr              strtabVirtual = 0;
    Elf64_Xword             strtabSize    = 0;
    Elf64_Addr              symtabVirtual = 0;
    Elf64_Addr              relaVirtual   = 0;
    Elf64_Xword             relaSize      = 0;
    Elf64_Addr              pltGotVirtual = 0;
    Elf64_Addr              jmpVirtual    = 0;
    uint64_t                jmpSize       = 0;
    Elf64_Addr              hashVirtual   = 0;
    for (size_t j = 0; j < entryCount; ++j) {
        Elf64_Dyn* dyn = new Elf64_Dyn;
        vfs::readFile(handle, sizeof(Elf64_Dyn), dyn);
#ifdef DEBUG
        dbg::printm(MODULE, "DYN[%lu] Tag: %lu Val: (0x%lx %lu)\n", j, dyn->d_tag, dyn->d_un.d_ptr,
                    dyn->d_un.d_val);
#endif
        if (dyn->d_tag == DT_NULL) {
            delete dyn;
            break;
        }
        if (dyn->d_tag >= DT_LOOS) {
            delete dyn;
            continue;
        }
        switch (dyn->d_tag) {
        case DT_NEEDED: {
            dtNeededEntries.push(dyn->d_un.d_val);
        } break;
        case DT_FINI:
        case DT_FINI_ARRAY:
        case DT_FINI_ARRAYSZ:
        case DT_INIT:
        case DT_INIT_ARRAY:
        case DT_INIT_ARRAYSZ: {
            dbg::printm(MODULE, "TODO: Add support init and fini Type: 0x%x, Value: 0x%lx\n",
                        dyn->d_tag, dyn->d_un.d_ptr);
        } break;
        case DT_STRTAB: {
            strtabVirtual = dyn->d_un.d_ptr + PHDRAddend;
        } break;
        case DT_STRSZ: {
            strtabSize = dyn->d_un.d_val;
        } break;
        case DT_SYMTAB: {
            symtabVirtual = dyn->d_un.d_ptr + PHDRAddend;
        } break;
        case DT_SYMENT: {
            if (dyn->d_un.d_val != 0x18) {
                dbg::printm(MODULE, "Symbol entry size isn't 24 bytes long!!!\n");
                std::abort();
            }
        } break;
        case DT_RELA: {
            relaVirtual = dyn->d_un.d_ptr + PHDRAddend;
        } break;
        case DT_RELASZ: {
            relaSize = dyn->d_un.d_val;
        } break;
        case DT_RELAENT: {
            if (dyn->d_un.d_val != 0x18) {
                dbg::printm(MODULE, "Rela entry size isn't 24 bytes long!!!\n");
                std::abort();
            }
        } break;
        case DT_HASH: {
            hashVirtual = dyn->d_un.d_ptr + PHDRAddend;
        } break;
        case DT_PLTRELSZ: {
            jmpSize = dyn->d_un.d_val;
        } break;
        case DT_PLTGOT: {
            pltGotVirtual = dyn->d_un.d_ptr + PHDRAddend;
        } break;
        case DT_JMPREL: {
            jmpVirtual = dyn->d_un.d_ptr + PHDRAddend;
        } break;
        case DT_PLTREL: {
            if (dyn->d_un.d_val != DT_RELA) {
                dbg::printm(MODULE, "ERROR: DT_PLTREL referincing REL instead of RELA\n");
                std::abort();
            }
        } break;
        // Optionals:
        case DT_DEBUG: {
        } break;
        default: {
            dbg::printm(MODULE, "TODO: Handle DYN[%u]: Type: 0x%x, Value: 0x%lx\n", j, dyn->d_tag,
                        dyn->d_un.d_ptr);
            // std::abort();
        } break;
        }
        delete dyn;
    }
    if (hashVirtual == 0) {
        dbg::printm(MODULE, "No hash virtual address specified!!!\n");
        std::abort();
    }
    if (symtabVirtual == 0) {
        dbg::printm(MODULE, "No symtab virtual address specified!!!\n");
        std::abort();
    }
    if (strtabVirtual == 0) {
        dbg::printm(MODULE, "No strtab virtual address specified!!!\n");
        std::abort();
    }
    if (strtabSize == 0) {
        dbg::printm(MODULE, "No strtab size found!!!\n");
        std::abort();
    }
    uint64_t strtabFileOffset = 0;
    for (task::Mapping* phdr : obj->mappings) {
        if (strtabVirtual >= phdr->virtualStart &&
            strtabVirtual < phdr->virtualStart + phdr->fileLength) {
            size_t offsetInSegment = strtabVirtual - phdr->virtualStart;
            strtabFileOffset       = phdr->fileOffset + offsetInSegment;
            break;
        }
    }
    if (strtabFileOffset == 0) {
        dbg::printm(MODULE, "Failed to aquire strtabFileOffset\n");
        std::abort();
    }
    uint8_t* strtab = new uint8_t[strtabSize];
    vfs::seek(handle, strtabFileOffset);
    vfs::readFile(handle, strtabSize, strtab);
    while (!dtNeededEntries.empty()) {
        size_t index = dtNeededEntries.front();
        dtNeededEntries.pop();
        if (index >= strtabSize) {
            dbg::printm(MODULE, "DT_NEEDED attempted access (0x%llx) from out of range string\n",
                        index);
            std::abort();
        }
        const char* file = makeStringFromStrTab(strtab, strtabSize, index);
        assert(file != nullptr);
        assert(strlen(file) > 0);
        std::string path = prefix + file;
        assert(path.size() > 0);
        uint64_t libHandle = vfs::openFile(path.c_str(), 0);
        if (libHandle == (uint64_t)-1) {
            dbg::printm(MODULE, "Unable to find needed dependency `%s`\n", path.c_str());
            delete obj;
            return nullptr;
        }
        uint64_t addressOffset = obj->mappings.at(obj->mappings.size() - 1)->virtualStart +
                                 obj->mappings.at(obj->mappings.size() - 1)->memLength;
        ElfObject* libObj = loadElfObject(libHandle, addressOffset);
        obj->dependencies.push_back(libObj);
    }
    if (relaVirtual == 0 || relaSize == 0) {
        dbg::printm(MODULE, "WARNING No rela found!\n");
    }
    obj->symtabVirtual = symtabVirtual;
    obj->hashVirtual   = hashVirtual;
    obj->pltGotVirtual = pltGotVirtual;
    obj->jmpVirtual    = jmpVirtual;
    obj->jmpSize       = jmpSize;
    obj->relaVirtual   = relaVirtual;
    obj->relaSize      = relaSize;
    obj->strtab        = strtab;
    obj->strsize       = strtabSize;
    delete dynPhdr;
    delete header;
    dbg::popTrace();
    return obj;
}
}; // namespace objects::elf