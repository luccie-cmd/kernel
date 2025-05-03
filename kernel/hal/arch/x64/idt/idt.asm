bits 64
section .text
global loadIDT
loadIDT:
    mov [rel IDT.limit], rsi
    mov [rel IDT.base], rdi
    lidt [rel IDT]
    ret

section .data
global IDT
IDT:
    .limit: dw 0
    .base: dq 0