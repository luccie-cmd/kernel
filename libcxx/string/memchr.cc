// #include <cstring>
#include <cstddef>
#include <cstdint>

extern "C" const void* memchr(const void* ptr, int ch, size_t count) {
    const uint8_t* u8ptr = (const uint8_t*)ptr;
    while (*u8ptr && count) {
        if (*u8ptr == ch) return u8ptr;

        ++u8ptr;
        --count;
    }
    return nullptr;
}