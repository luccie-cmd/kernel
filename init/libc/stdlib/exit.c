#include <__syscall.h>
#include <stdlib.h>

__attribute__((noreturn)) void exit(int status) {
    syscall_execute(SYS_EXIT, status & 0xFF);
    __builtin_unreachable();
}

void __ubsan_handle_builtin_unreachable() {
    exit(12);
}