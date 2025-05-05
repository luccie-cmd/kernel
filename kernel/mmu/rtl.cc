/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <common/dbg/dbg.h>
#include <cstring>
#include <kernel/hal/arch/x64/idt/idt.h>
#include <kernel/mmu/heap/heap.h>
#include <new>

void* operator new(size_t size)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    void* addr = mmu::heap::allocate(size);
    std::memset(addr, 0, size);
    dbg::popTrace();
    return addr;
}
void* operator new(size_t size, std::align_val_t align)
{
    (void)align;
    dbg::addTrace(__PRETTY_FUNCTION__);
    void* addr = mmu::heap::allocate(size);
    std::memset(addr, 0, size);
    dbg::popTrace();
    return addr;
}
void* operator new[](size_t size)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    void* addr = mmu::heap::allocate(size);
    std::memset(addr, 0, size);
    dbg::popTrace();
    return addr;
}
void operator delete[](void* ptr, size_t size)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr, size);
    dbg::popTrace();
}
void operator delete[](void* ptr)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr);
    dbg::popTrace();
}
void operator delete(void* ptr, size_t size)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr, size);
    dbg::popTrace();
}
void operator delete(void* ptr)
{
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr);
    dbg::popTrace();
}
void operator delete(void* ptr, size_t size, std::align_val_t align)
{
    (void)align;
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr, size);
    dbg::popTrace();
}