extern main
extern __libc_init_main
global _start
_start:
    xor rbp, rbp
    lea rdi, [rel main]
    mov rsi, [rsp]
    lea rdx, [rsp + 8]
    call __libc_init_main
    hlt