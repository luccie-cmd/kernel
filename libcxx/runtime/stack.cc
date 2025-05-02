#include <common/dbg/dbg.h>
#include <cstdlib>

extern "C" void __stack_chk_fail(){
    dbg::printf("* Stack smashing detected *\n");
    std::abort();
}

// extern "C" void _Unwind_Resume(struct _Unwind_Exception * object){
//     (void)object;
//     dbg::printf("* Exception resumed *\n");
//     std::abort();
// }