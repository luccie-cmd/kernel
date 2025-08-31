#include <common/dbg/dbg.h>
#include <common/io/io.h>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <drivers/display.h>
#include <kernel/driver/driver.h>
extern drivers::DisplayDriver* displayDriver;

namespace dbg {
const char* messages[8192];
uint16_t    messagesCount = 0;
static void putchar(const char c) {
    io::outb(DBG_PORT, c);
    // if (displayDriver != nullptr) {
    //     displayDriver->drawChar(0, c);
    // }
}
static void puts(const char* str) {
    while (*str) {
        putchar(*str);
        str++;
    }
    // if (displayDriver == nullptr)
    // {
    //     messages[messagesCount++] = str;
    //     return;
    // }
    // if (messagesCount > 0)
    // {
    //     for (const char *mess : messages)
    //     {
    // while (*mess)
    // {
    //     putchar(*mess);
    //     mess++;
    // }
    //     }
    //     messagesCount = 0;
    // }
}
void print(const char* str) {
    puts(str);
}
char          str[8192];
std::Spinlock strLock;
void          printv(const char* fmt, va_list args) {
    strLock.lock();
    std::memset(str, 0, sizeof(str));
    std::vsnprintf(str, sizeof(str), fmt, args);
    print(str);
    strLock.unlock();
}
void printf(const char* fmt, ...) {
    std::va_list args;
    va_start(args, fmt);
    printv(fmt, args);
    va_end(args);
}
extern "C" void dbgPrintf(const char* fmt, ...) {
    std::va_list args;
    va_start(args, fmt);
    printv(fmt, args);
    va_end(args);
}
char          outStr[8192];
std::Spinlock outStrlock;
void          printm(const char* module, const char* fmt, ...) {
    outStrlock.lock();
    const char* fmtString = "%s: %s";
    std::memset(str, 0, sizeof(str));
    std::memset(outStr, 0, sizeof(outStr));
    snprintf(str, sizeof(str), fmtString, module, fmt);
    va_list args;
    va_start(args, fmt);
    vsnprintf(outStr, sizeof(outStr), str, args);
    print(outStr);
    va_end(args);
    outStrlock.unlock();
}
// static const char* stackTraces[8192];
// static uint16_t    nstackTraces = 0;
void addTrace(const char* func) {
    (void)func;
    // if (nstackTraces >= sizeof(stackTraces) / sizeof(stackTraces[0])) {
    //     std::memset(stackTraces, 0, sizeof(stackTraces));
    //     nstackTraces = 0;
    //     dbg::printf("ERROR: Too many stack traces\n");
    //     std::abort();
    // }
    // stackTraces[nstackTraces++] = func;
}
void popTrace() {
    // if (nstackTraces == 0) {
    //     dbg::printf("WARNING: Attempted to pop from empty stack trace\n");
    //     return;
    // }
    // --nstackTraces;
}
void printStackTrace() {
    // for (uint16_t i = 0; i < nstackTraces; ++i) {
    //     const char* trace = stackTraces[i];
    //     if (trace == nullptr) {
    //         break;
    //     }
    //     print(trace);
    //     putchar('\n');
    // }
}
std::vector<const char*> getMessages() {
    return {};
}
} // namespace dbg