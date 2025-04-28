# 0 "kernel/hal/arch/x64/gdt/gdt.asm"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "kernel/hal/arch/x64/gdt/gdt.asm"
bits 64
section .text
global loadGDT
loadGDT:
    mov [GDT.limit], rsi
    mov [GDT.base], rdi
    lgdt [GDT]
    ; Reload segments to kernel cs and ds.
.reloadSegments:
    push 0x08
    lea rax, [rel .reload_CS]
    push rax
    retfq
.reload_CS:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

section .data
global GDT
GDT:
    .limit: dw 0
    .base: dq 0
