#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdlib>
#include <kernel/driver/driver.h>
#include <kernel/mmu/mmu.h>
#include <kernel/vfs/vfs.h>
#include <vector>

extern "C" void abort()
{
    dbg::print("INFO:\n");
    vfs::printInfo();
    mmu::printInfo();
    driver::printInfo();
    dbg::print("ABORTING KERNEL\n");
    dbg::print("STACK TRACE:\n");
    dbg::printStackTrace();
    while (1)
    {
        // io::cli();
        __asm__("hlt");
    }
}