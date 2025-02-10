#include <cxxabi.h>
#include <vector>
#include <utility>
#include <common/dbg/dbg.h>

void* __dso_handle = nullptr;

extern "C" int __cxa_atexit(void (*destructor)(void*), void* arg, void* dso_handle){
    dbg::print("__cxa_atexit called\n");
    return 0;
}