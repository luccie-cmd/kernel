global switchProc
section .trampoline.text
switchProc:
    mov rbx, rdi
    mov r12, rsi
    mov r13, rdx

    mov rcx, 0xC0000080
	rdmsr
    or rax, 1
	wrmsr

    mov rcx, 0xc0000081
	rdmsr
	mov edx, 0x00100008
	wrmsr

    mov rax, 0x1B
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov rsp, rbx
    mov rbp, rbx
    mov rcx, r12
    mov r11, 0x202
    mov cr3, r13

    xor rax, rax
    xor rbx, rbx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

    o64 sysret