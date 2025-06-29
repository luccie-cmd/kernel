#include <common/dbg/dbg.h>
#include <cstring>

extern "C" void printf(const char* fmt, ...) {
    std::va_list args;
    va_start(args, fmt);
    dbg::printv(fmt, args);
    va_end(args);
}