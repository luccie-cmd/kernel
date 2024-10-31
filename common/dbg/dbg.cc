#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstddef>

namespace dbg{
    static void putchar(const char c){
        dbg::addTrace(__PRETTY_FUNCTION__);
        io::outb(DBG_PORT, c);
        dbg::popTrace();
    }
    static void puts(const char* str){
        dbg::addTrace(__PRETTY_FUNCTION__);
        while(*str){
            putchar(*str);
            str++;
        }
        dbg::popTrace();
    }
    void print(const char* str){
        dbg::addTrace(__PRETTY_FUNCTION__);
        puts(str);
        dbg::popTrace();
    }
    void printm(const char* str, const char* module){
        dbg::addTrace(__PRETTY_FUNCTION__);
        puts(module);
        puts(": ");
        puts(str);
        dbg::popTrace();
    }
    const char* stackTraces[4096];
    size_t nstackTraces = 0;
    void addTrace(const char* func){
        stackTraces[nstackTraces++] = func;
    }
    void popTrace(){
        nstackTraces--;
    }
    void printStackTrace(){
        for(size_t i = 0; i < nstackTraces; ++i){
            print(stackTraces[i]);
            putchar('\n');
        }
    }
}