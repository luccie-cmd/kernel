#include <cstdlib>
#include <common/dbg/dbg.h>
#include <common/io/io.h>

void abort(){
    dbg::print("ABORTING KERNEL\n");
    dbg::print("STACK TRACE:\n");
    dbg::printStackTrace();
    io::cli();
    while(1){
        __asm__("hlt");
    }
}