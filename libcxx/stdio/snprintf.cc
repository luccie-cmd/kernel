#include <cstdio>
#include <cstdarg>
#include <common/dbg/dbg.h>

extern "C" int snprintf(char *s, size_t maxlen, const char *format, ...){
    dbg::addTrace(__PRETTY_FUNCTION__);
    va_list args;
    va_start(args, format);
    int result = std::vsnprintf(s, maxlen, format, args);
    va_end(args);
    dbg::popTrace();
    return result;
}