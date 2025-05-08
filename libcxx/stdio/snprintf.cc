#include <cstdarg>
#include <cstdio>

extern "C" int snprintf(char* s, size_t maxlen, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = std::vsnprintf(s, maxlen, format, args);
    va_end(args);
    return result;
}