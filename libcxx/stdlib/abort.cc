#include <cstdlib>
#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <vector>
#include <kernel/mmu/mmu.h>
#include <kernel/vfs/vfs.h>
#include <kernel/driver/driver.h>

extern "C" void abort(){
    dbg::print("INFO:\n");
    vfs::printInfo();
    mmu::printInfo();
    driver::printInfo();
    dbg::print("ABORTING KERNEL\n");
    dbg::print("STACK TRACE:\n");
    dbg::printStackTrace();
    while(1){
        io::cli();
        __asm__("hlt");
    }
}