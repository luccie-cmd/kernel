#include <cstring>

void* memmove(void* destination, const void* source, size_t num) {
    const char* src = (const char*)source;
    char*       dst = (char*)destination;
    while (num--) {
        dst[num] = src[num];
    }
    return destination;
}