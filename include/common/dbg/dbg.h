#if !defined(_COMMON_DBG_DBG_H_)
#define _COMMON_DBG_DBG_H_

namespace dbg{
    void printm(const char* str, const char* module);
    void print(const char* str);
    void printStackTrace();
    void addTrace(const char* func);
    void popTrace();
};

#endif // _COMMON_DBG_DBG_H_
