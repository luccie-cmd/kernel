#include <common/io/io.h>
#include <common/dbg/dbg.h>

namespace io{
    void outb(uint16_t port, uint8_t value){
        __asm__ volatile ( "outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory" );
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