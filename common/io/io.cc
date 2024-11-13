#include <common/io/io.h>
#include <common/dbg/dbg.h>

namespace io{
    void outb(uint16_t port, uint8_t value){
        __asm__ volatile ( "outb %0, %1" : : "a"(value), "Nd"(port) : "memory" );
    }
    void outl(uint16_t port, uint32_t value){
        __asm__ volatile ( "outl %0, %1" : : "a"(value), "Nd"(port) : "memory" );
    }
    uint8_t inb(uint16_t port){
        uint8_t ret;
        __asm__ volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
        return ret;
    }
    uint32_t inl(uint16_t port){
        uint32_t ret;
        __asm__ volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(port) : "memory" );
        return ret;
    }
    void insl(uint16_t port, void *buffer, uint32_t count){
        __asm__ volatile ( "cld; rep insl" : "+D" (buffer), "+c" (count) : "d" (port) : "memory");
    }
    void cli(){
        __asm__ volatile ( "cli" );
    }
    void sti(){
        __asm__ volatile ( "sti" );
    }
    void invalpg(void* addr){
        __asm__ volatile ( "invlpg (%0)" : : "r"(addr) : "memory" );
    }
    void wcr3(uint64_t newCR3){
        __asm__ volatile ( "mov %0, %%cr3" : : "r" (newCR3) : "memory" );
    }
    uint64_t rcr3(){
        uint64_t cr3;
        __asm__ volatile ( "mov %%cr3, %0" : "=r"(cr3) );
        return cr3;
    }
};