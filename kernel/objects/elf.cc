#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <elf.h>
#include <kernel/mmu/vmm/vmm.h>
#include <kernel/objects/elf.h>
#include <kernel/vfs/vfs.h>
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
    Elf64_Half phentsize = header->e_phentsize;
    Elf64_Half phnum     = header->e_phnum;
    for (Elf64_Half i = 0; i < phnum; i++) {
        Elf64_Phdr* phdr = new Elf64_Phdr;
        vfs::readFile(handle, phentsize, phdr);
        if (phdr->p_type != PT_LOAD) {
            delete phdr;
            continue;
        }
        dbg::printm(MODULE,
                    "PHDR[%u]: Type: 0x%x, VAddr: 0x%lx, PAddr: 0x%lx, FileSz: 0x%lx, MemSz: "
                    "0x%lx, Flags: 0x%x, Align: 0x%lx\n",
                    i, phdr->p_type, phdr->p_vaddr, phdr->p_paddr, phdr->p_filesz, phdr->p_memsz,
                    phdr->p_flags, phdr->p_align);
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
        mapping->virtualStart = ALIGNUP(phdr->p_vaddr, phdr->p_align == 0 ? 1 : phdr->p_align);
        delete phdr;
        obj->mappings.push_back(mapping);
    }
    Elf64_Off shoff = header->e_shoff;
    vfs::seek(handle, shoff);
    Elf64_Half shentsize = header->e_shentsize;
    Elf64_Half shnum     = header->e_shnum;
    for (Elf64_Half i = 0; i < shnum; i++) {
        Elf64_Shdr* shdr = new Elf64_Shdr;
        vfs::readFile(handle, shentsize, shdr);
        dbg::printm(MODULE,
                    "SHDR[%u]: Type: 0x%x, Addr: 0x%lx, Offset: 0x%lx, Size: 0x%lx, Flags: 0x%lx, "
                    "Align: 0x%lx\n",
                    i, shdr->sh_type, shdr->sh_addr, shdr->sh_offset, shdr->sh_size, shdr->sh_flags,
                    shdr->sh_addralign);
        delete shdr;
    }
    delete header;
    dbg::popTrace();
    return obj;
}
}; // namespace objects::elf