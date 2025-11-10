#if !defined(__LIBC_SYSCALL_H__)
#define __LIBC_SYSCALL_H__
#include <stdint.h>
#define SYS_EXIT 0

uint64_t syscall_execute(uint64_t code, ...);

#endif // __LIBC_SYSCALL_H__
