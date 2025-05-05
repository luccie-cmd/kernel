#include <common/dbg/dbg.h>
#include <cstdarg>
#include <cstdlib>
#include <new>

namespace std
{
void __throw_length_error(char const* str)
{
    dbg::printf("%s:%s\n", __PRETTY_FUNCTION__, str);
    std::abort();
}
void __throw_bad_alloc()
{
    dbg::printf("%s\n", __PRETTY_FUNCTION__);
    std::abort();
}
void __throw_bad_array_new_length()
{
    dbg::printf("%s\n", __PRETTY_FUNCTION__);
    std::abort();
}
void __throw_out_of_range(char const* fmt)
{
    dbg::printf("%s:%s\n", __PRETTY_FUNCTION__, fmt);
    std::abort();
}
void __throw_logic_error(char const* fmt)
{
    dbg::printf("%s:%s\n", __PRETTY_FUNCTION__, fmt);
    std::abort();
}
void __throw_out_of_range_fmt(char const* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    dbg::printv(fmt, args);
    va_end(args);
    dbg::print("\n");
    std::abort();
}
}; // namespace std