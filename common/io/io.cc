#include <common/io/io.h>
#include <common/dbg/dbg.h>

namespace io{
    void outb(uint16_t port, uint8_t value){
        dbg::addTrace(__PRETTY_FUNCTION__);
        __asm__ volatile ( "outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory" );
        dbg::popTrace();
    }
    void cli(){
        dbg::addTrace(__PRETTY_FUNCTION__);
        __asm__ volatile ( "cli" );
        dbg::popTrace();
    }
};