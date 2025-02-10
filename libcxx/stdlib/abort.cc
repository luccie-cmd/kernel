#include <cstdlib>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <vector>

extern "C" void abort(){
    dbg::print("ABORTING KERNEL\n");
    dbg::print("STACK TRACE:\n");
    dbg::printStackTrace();
    while(1){
        io::cli();
        __asm__("hlt");
    }
}