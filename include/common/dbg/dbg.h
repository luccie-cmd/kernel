#if !defined(_COMMON_DBG_DBG_H_)
#define _COMMON_DBG_DBG_H_
#include <cstdarg>
#include <vector>

namespace dbg{
    void printm(const char* module, const char* fmt, ...);
    void printv(const char* fmt, va_list args);
    void printf(const char* str, ...);
    void print(const char* str);
    void printStackTrace();
    void addTrace(const char* func);
    void popTrace();
    std::vector<const char*> getMessages();
};

#endif // _COMMON_DBG_DBG_H_
