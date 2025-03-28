#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

namespace dbg{
    std::vector<const char*> messages;
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
        messages.push_back(str);
    }
    void printv(const char* fmt, va_list args){
        char str[4096];
        std::vsnprintf(str, sizeof(str), fmt, args);
        print(str);
    }
    void printf(const char* fmt, ...){
        std::va_list args;
        va_start(args, fmt);
        printv(fmt, args);
        va_end(args);
    }
    void printm(const char* module, const char* fmt, ...){
        const char* fmtString = "%s: %s";
        char str[4096];
        char outStr[4096];
        snprintf(str, sizeof(str), fmtString, module, fmt);
        va_list args;
        va_start(args, fmt);
        vsnprintf(outStr, sizeof(outStr), str, args);
        print(outStr);
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
    std::vector<const char*> getMessages(){
        return messages;
    }
}