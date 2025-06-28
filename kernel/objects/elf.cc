#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <elf.h>
#include <kernel/mmu/vmm/vmm.h>
#include <kernel/objects/elf.h>
#include <kernel/vfs/vfs.h>
#include <queue>
#define MODULE "ELF Loader"

namespace objects::elf {
static bool verifyHeader(Elf64_Ehdr* hdr) {
    if (hdr->e_ident[EI_MAG0] != ELFMAG0 || hdr->e_ident[EI_MAG1] != ELFMAG1 ||
        hdr->e_ident[EI_MAG2] != ELFMAG2 || hdr->e_ident[EI_MAG3] != ELFMAG3) {
        return false;
    }
    return true;
}
ElfObject* loadElfObject(int handle) {
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
    ElfObject* obj     = new ElfObject;
    obj->type          = header->e_type;
    obj->entryPoint    = header->e_entry;
    Elf64_Off phOffset = header->e_phoff;
    vfs::seek(handle, phOffset);
    Elf64_Half  phentsize = header->e_phentsize;
    Elf64_Half  phnum     = header->e_phnum;
    Elf64_Phdr* dynPhdr   = nullptr;
    for (Elf64_Half i = 0; i < phnum; i++) {
        Elf64_Phdr* phdr = new Elf64_Phdr;
        vfs::readFile(handle, phentsize, phdr);
        if (phdr->p_type != PT_LOAD && phdr->p_type != PT_DYNAMIC) {
            delete phdr;
            continue;
        }
        dbg::printm(MODULE,
                    "PHDR[%u]: Type: 0x%x, VAddr: 0x%lx, PAddr: 0x%lx, FileSz: 0x%lx, MemSz: "
                    "0x%lx, Flags: 0x%x, Align: 0x%lx, Offset: 0x%lx\n",
                    i, phdr->p_type, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz,
                    phdr->p_flags, phdr->p_align, phdr->p_offset);
        if (phdr->p_type == PT_LOAD) {
            task::Mapping* mapping = new task::Mapping;
            mapping->fileLength    = phdr->p_filesz;
            mapping->fileOffset    = phdr->p_offset;
            mapping->memLength     = phdr->p_memsz;
            if ((phdr->p_flags & PF_X) == 0) {
                mapping->permissions |= PROTECTION_NOEXEC;
            }
            if ((phdr->p_flags & PF_R) != 0 || (phdr->p_flags & PF_W) != 0) {
                mapping->permissions |= PROTECTION_RW;
            }
            mapping->virtualStart = phdr->p_vaddr;
            mapping->alignment    = phdr->p_align;
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
        std::abort();
    }
    task::Mapping* mapping = new task::Mapping;
    mapping->fileLength    = dynPhdr->p_filesz;
    mapping->fileOffset    = dynPhdr->p_offset;
    mapping->memLength     = dynPhdr->p_memsz;
    mapping->permissions   = PROTECTION_NOEXEC | PROTECTION_RW;
    mapping->virtualStart  = dynPhdr->p_vaddr;
    mapping->alignment     = dynPhdr->p_align;
    obj->dynamicSection    = mapping;
    vfs::seek(handle, dynPhdr->p_offset);
    size_t                  entryCount = dynPhdr->p_filesz / sizeof(Elf64_Dyn);
    std::queue<Elf64_Xword> dtNeededEntries;
    for (size_t j = 0; j < entryCount; ++j) {
        Elf64_Dyn* dyn = new Elf64_Dyn;
        vfs::readFile(handle, sizeof(Elf64_Dyn), dyn);
        if (dyn->d_tag == DT_NULL) {
            delete dyn;
            break;
        }
        switch (dyn->d_tag) {
        case DT_NEEDED: {
            dtNeededEntries.push(dyn->d_un.d_val);
        } break;
        case DT_FINI:
        case DT_INIT: {
            dbg::printm(MODULE, "TODO: Add support for DYN[%u]: Type: 0x%x, Value: 0x%lx\n", j,
                        dyn->d_tag, dyn->d_un.d_ptr);
        } break;
        default: {
            dbg::printm(MODULE, "TODO: Handle DYN[%u]: Type: 0x%x, Value: 0x%lx\n", j, dyn->d_tag,
                        dyn->d_un.d_ptr);
            // std::abort();
        } break;
        }
        delete dyn;
    }
    delete dynPhdr;
    delete header;
    dbg::popTrace();
    return obj;
}
}; // namespace objects::elf