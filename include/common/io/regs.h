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
    uint64_t cr3, gs, fs, es, ds, r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rdx, rcx,
        rbx, _, returnIP, userCr3, rax, interrupt_number, error_code, rip, cs, rflags, orig_rsp, ss;
} __attribute__((packed));
} // namespace io

#endif // _COMMON_IO_REGS_H_
