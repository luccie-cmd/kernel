#include <cstring>

extern "C" const char* strchr(const char* str, char chr){
    while (*str){
        if (*str == chr)
            return str;

        ++str;
    }
    return nullptr;
}