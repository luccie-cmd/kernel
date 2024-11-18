#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

namespace dbg{
    static void putchar(const char c){
        io::outb(DBG_PORT, c);
    }
    static void puts(const char* str){
        while(*str){
            putchar(*str);
            str++;
        }
    }
    void print(const char* str){
        puts(str);
    }
    void printf(const char* fmt, ...){
        char str[4096];
        std::va_list args;
        va_start(args, fmt);
        std::vsnprintf(str, sizeof(str), fmt, args);
        print(str);
        va_end(args);
    }
    void printm(const char* module, const char* fmt, ...){
        print(module);
        print(": ");
        char str[4096];
        std::va_list args;
        va_start(args, fmt);
        std::vsnprintf(str, sizeof(str), fmt, args);
        print(str);
        va_end(args);
    }
    static const char* stackTraces[4096];
    static uint16_t nstackTraces = 0;
    void addTrace(const char* func){
        stackTraces[nstackTraces++] = func;
    }
    void popTrace(){
        nstackTraces--;
    }
    void printStackTrace(){
        for(uint16_t i = 0; i < nstackTraces; ++i){
            const char* trace = stackTraces[i];
            if(trace == nullptr){
                break;
            }
            print(trace);
            putchar('\n');
        }
    }
}