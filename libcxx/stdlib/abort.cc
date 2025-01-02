#include <cstdlib>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <kernel/info.h>

extern "C" void abort(){
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::print("ABORTING KERNEL\n");
    dbg::print("INFO:\n");
    info::printInfo();
    dbg::print("STACK TRACE:\n");
    dbg::printStackTrace();
    while(1){
        io::cli();
        __asm__("hlt");
    }
}