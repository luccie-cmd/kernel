/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <kernel/mmu/heap/heap.h>

void* operator new[](size_t size){
    return mmu::heap::allocate(size);
}

void operator delete[](void* ptr, size_t size){
    mmu::heap::free(ptr, size);
}

void operator delete[](void* ptr){
    mmu::heap::free(ptr);
}