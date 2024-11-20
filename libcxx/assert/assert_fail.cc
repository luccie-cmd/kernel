#include <cassert>
#include <common/dbg/dbg.h>
#include <cstdlib>

void __assert_fail (const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function){
    dbg::addTrace(__PRETTY_FUNCTION__);
    dbg::printf("%s:%ld:%s: Assertion %s failed!\n", __file, __line, __function, __assertion);
    std::abort();
}