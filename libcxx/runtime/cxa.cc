#include <bits/allocator.h>
#include <common/dbg/dbg.h>
#include <cstdlib>
#include <cxxabi.h>

void* __dso_handle = nullptr;

extern "C" int __cxa_atexit(void (*destructor)(void*), void* arg, void* dso_handle) {
    (void)destructor;
    (void)arg;
    (void)dso_handle;
    dbg::print("__cxa_atexit called\n");
    return 0;
}

extern "C" void __cxa_pure_virtual() {
    dbg::printf("__cxa_pure_virtual\n");
    std::abort();
}

extern "C" int __cxa_guard_acquire(__cxxabiv1::__guard* guard_object) {
    (void)guard_object;
    return 0;
}

extern "C" void __cxa_guard_release(__cxxabiv1::__guard* guard_object) {
    (void)guard_object;
}