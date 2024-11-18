#if !defined(_COMMON_DBG_DBG_H_)
#define _COMMON_DBG_DBG_H_

namespace dbg{
    void printm(const char* module, const char* fmt, ...);
    void printf(const char* str, ...);
    void print(const char* str);
    void printStackTrace();
    void addTrace(const char* func);
    void popTrace();
};

#endif // _COMMON_DBG_DBG_H_
