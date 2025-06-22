/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <../limine/limine.h>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <kernel/mmu/mmu.h>
#define MODULE "MMU PMM"

namespace mmu::pmm {
static bool                                                        __initialized  = false;
limine_memmap_request __attribute__((section(".limine_requests"))) memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr};
static node* __head         = nullptr;
uint64_t     total          = 0;
uint64_t     allocatedPages = 0;
void         initialize() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (memmap_request.response == nullptr) {
        dbg::printm(MODULE, "Bootloader failed to set memory map response\n");
        std::abort();
    }
    uint64_t memmap_entries = memmap_request.response->entry_count;
    for (uint64_t i = 0; i < memmap_entries; ++i) {
        limine_memmap_entry* entry = memmap_request.response->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= PAGE_SIZE) {
            if (entry->base == io::rcr3()) {
                continue;
            }
            total += entry->length;
            node* newNode = (node*)vmm::makeVirtual(entry->base);
            newNode->size = entry->length;
            if (__head == nullptr) {
                __head = newNode;
            } else {
                newNode->next = __head;
                __head        = newNode;
            }
        }
    }
    __initialized = true;
    dbg::popTrace();
}
bool isInitialized() {
    return __initialized;
}
void free(uint64_t addr) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    node* current = __head;
    while (current->next != nullptr) {
        current = current->next;
    }
    node* newNode = (node*)(addr + vmm::getHHDM());
    newNode->size = current->size;
    newNode->next = current->next;
    current->next = newNode;
    allocatedPages -= 1;
    dbg::popTrace();
}
uint64_t allocVirtual(uint64_t size) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    size          = ALIGNUP(size, PAGE_SIZE);
    node* current = __head;
    node* prev    = nullptr;
    while (current) {
        if (current->size >= size) {
            uint64_t addr = (uint64_t)current;
            if (current->size > size) {
                node* newNode = (node*)(addr + size);
                newNode->size = current->size - size;
                newNode->next = current->next;
                if (prev != nullptr) {
                    prev->next = newNode;
                } else {
                    __head = newNode;
                }
            } else {
                if (prev != nullptr) {
                    prev->next = current->next;
                } else {
                    __head = current->next;
                }
            }
            allocatedPages += size / PAGE_SIZE;
            dbg::popTrace();
            return addr - vmm::getHHDM();
        }
        prev    = current;
        current = current->next;
    }
    dbg::printm(MODULE, "Could not find a physical address for size 0x%llx\n", size);
    std::abort();
    dbg::popTrace();
}
uint64_t allocate() {
    dbg::addTrace(__PRETTY_FUNCTION__);
    uint64_t addr = allocVirtual(PAGE_SIZE);
    dbg::popTrace();
    return addr;
}
void printInfo() {
    dbg::printm(MODULE, "INFO\n");
    dbg::printm(MODULE, "Found pages: %lu\n", total / PAGE_SIZE);
    dbg::printm(MODULE, "Allocated pages: %lu\n", allocatedPages);
}
} // namespace mmu::pmm