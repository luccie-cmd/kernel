#include <common/dbg/dbg.h>
#include <cstdlib>

extern "C" void exit(int __status)
{
    std::vector<const char*> messages = dbg::getMessages();
    for (int i = 0; i < __status; ++i)
    {
        dbg::printf("%s", messages.at(messages.size() - (i + 1)));
    }
    std::abort();
}