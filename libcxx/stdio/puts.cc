#include <common/dbg/dbg.h>
#include <cstring>

extern "C" int puts(const char* str)
{
    dbg::print(str);
    return std::strlen(str);
}