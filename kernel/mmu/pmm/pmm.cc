/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <common/spinlock.h>
#include <cstdlib>
#include <kernel/mmu/mmu.h>
#include <limine.h>
#define MODULE "MMU PMM"

namespace mmu::pmm {
static bool                                                        __initialized  = false;
limine_memmap_request __attribute__((section(".limine_requests"))) memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr};
static node*  __head = nullptr;
std::Spinlock headSpinlock;
uint64_t      total          = 0;
uint64_t      allocatedPages = 0;
void          initialize() {
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
void free(uint64_t addr, uint64_t size) {
    headSpinlock.lock();
    if (!iscanonical(addr)) {
        dbg::printm(MODULE, "Free address of 0x%lx not canonical\n", addr);
        std::abort();
    }
#ifdef DEBUG
    dbg::printm(MODULE, "Freeing address 0x%lx\n", addr);
#endif
    size          = ALIGNUP(size, PAGE_SIZE);
    node* newNode = (node*)(addr + mmu::vmm::getHHDM());
    if ((uint64_t)newNode < mmu::vmm::getHHDM()) {
        dbg::printf("0x%lx, 0x%lx, %lp\n", addr, mmu::vmm::getHHDM(), newNode);
        std::abort();
    }
    newNode->size = size;
    node* current = __head;
    node* prev    = nullptr;
    while (current && current < newNode) {
        prev    = current;
        current = current->next;
    }
    newNode->next = current;
    if (prev) {
        prev->next = newNode;
    } else {
        __head = newNode;
    }
    allocatedPages -= size / PAGE_SIZE;
    headSpinlock.unlock();
}
uint64_t allocVirtual(uint64_t size) {
    dbg::addTrace(__PRETTY_FUNCTION__);
    if (!isInitialized()) {
        initialize();
    }
    headSpinlock.lock();
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
            dbg::printStackTrace();
#ifdef DEBUG
            dbg::printm(MODULE, "Allocated address 0x%lx\n", addr - vmm::getHHDM());
#endif
            dbg::popTrace();
            headSpinlock.unlock();
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