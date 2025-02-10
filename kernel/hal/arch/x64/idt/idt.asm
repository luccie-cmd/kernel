bits 64
section .text
global loadIDT
loadIDT:
    mov [IDT.limit], rsi
    mov [IDT.base], rdi
    lidt [IDT]
    ret

section .data
global IDT
IDT:
    .limit: dw 0
    .base: dq 0