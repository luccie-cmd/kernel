#if !defined(_KERNEL_TASK_SYSCALLS_WRITE_H_)
#define _KERNEL_TASK_SYSCALLS_WRITE_H_
#include <cstddef>
#include <cstdint>
#define SYSCALL_EXIT 0
#define SYSCALL_WRITE 1
#define SYSCALL_WRITESCREEN 3

namespace task::syscall {
struct SyscallRegs {
    uint64_t rbx, rip, rdx, rbp, rsi, rdi, r8, r9, r10, rflags, r12, r13, r14, r15, cr3, rax;
} __attribute__((packed));
size_t sysWrite(SyscallRegs* regs);
size_t sysExit(SyscallRegs* regs);
}; // namespace task::syscall

#endif // _KERNEL_TASK_SYSCALLS_WRITE_H_
