#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstddef>
#include <cstdlib>
#include <stl/vector>

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
    void printm(const char* str, const char* module){
        puts(module);
        puts(": ");
        puts(str);
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