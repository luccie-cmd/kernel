/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/mmu/heap/heap.h>
#include <common/dbg/dbg.h>
#include <cstring>
#include <kernel/hal/arch/x64/idt/idt.h>
#include <new>

void* operator new(size_t size){
    dbg::addTrace(__PRETTY_FUNCTION__);
    void* addr = mmu::heap::allocate(size);
    std::memset(addr, 0, size);
    dbg::popTrace();
    return addr;
}
void* operator new(size_t size, const std::nothrow_t& nothrow){
    (void)nothrow;
    dbg::addTrace(__PRETTY_FUNCTION__);
    void* addr = mmu::heap::allocate(size);
    std::memset(addr, 0, size);
    dbg::popTrace();
    return addr;
}
void* operator new[](size_t size){
    dbg::addTrace(__PRETTY_FUNCTION__);
    void* addr = mmu::heap::allocate(size);
    std::memset(addr, 0, size);
    dbg::popTrace();
    return addr;
}
void operator delete[](void* ptr, size_t size){
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr, size);
    dbg::popTrace();
}
void operator delete[](void* ptr){
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr);
    dbg::popTrace();
}
void operator delete(void* ptr, size_t size){
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr, size);
    dbg::popTrace();
}
void operator delete(void* ptr){
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr);
    dbg::popTrace();
}
void operator delete(void* ptr, const std::nothrow_t& nothrow){
    (void)nothrow;
    dbg::addTrace(__PRETTY_FUNCTION__);
    mmu::heap::free(ptr);
    dbg::popTrace();
}