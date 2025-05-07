#include <common/dbg/dbg.h>
#include <cstdlib>

extern "C" void __stack_chk_fail()
{
    dbg::printf("* Stack smashing detected *\n");
    std::abort();
}