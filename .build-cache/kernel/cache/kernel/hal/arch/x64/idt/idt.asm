# 0 "kernel/hal/arch/x64/idt/idt.asm"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "kernel/hal/arch/x64/idt/idt.asm"
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
