#include <cstdint>
#include <cstring>

extern "C" void* memcpy(void* dst, const void* src, std::size_t num)
{
    char*       u8Dst = (char*)dst;
    const char* u8Src = (const char*)src;
    for (std::size_t i = 0; i < num; i++)
        u8Dst[i] = u8Src[i];
    return dst;
}