/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_COMMON_IO_REGS_H_)
#define _COMMON_IO_REGS_H_
#include <cstdint>

namespace io {
struct Registers {
        //   0   8  16   24  32   40   48    56   64  72   80  88  96   104  112  120  128
    uint64_t gs, fs, es, ds, r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rdx, rcx,
    //  136 144  152      160  168  176                184        192  200  208     216      224
        rbx, _, returnIP, cr3, rax, interrupt_number, error_code, rip, cs, rflags, orig_rsp, ss;
} __attribute__((packed));
} // namespace io

#endif // _COMMON_IO_REGS_H_
