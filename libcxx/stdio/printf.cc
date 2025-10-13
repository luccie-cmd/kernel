#include <common/dbg/dbg.h>
#include <cstring>

extern "C" int printf(const char* fmt, ...) {
    std::va_list args;
    va_start(args, fmt);
    dbg::printv(fmt, args);
    va_end(args);
    return 0;
}