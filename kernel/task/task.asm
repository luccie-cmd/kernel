global switchProc
global syscallEntry
extern kernelCR3
extern tssRSP0
extern tempValue
extern syscallHandler
section .trampoline.text
syscallEntry:
    ; Uh yeah this could go very badly if the user decided to call syscall with RSP set to 0 so FIXME ig?
    mov [tempValue], rsp
    mov rsp, [tssRSP0]
    push rax
    mov rax, cr3
    push rax
    mov rax, [kernelCR3]
    mov cr3, rax

    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rdi
    push rsi
    push rbp
    push rdx
    push rcx
    push rbx

    lea rdi, [rsp]
    jmp syscallHandler
    
switchProc:
    mov rax, [rdi + 160]
    mov cr3, rax
    jmp .flush
.flush:
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

    mov rbx, [rdi + 136]
    mov rcx, [rdi + 192]
    mov rdx, [rdi + 120]
    mov rbp, [rdi + 112]
    mov rsp, [rdi + 216]
    mov rsi, [rdi + 104]
    mov r8,  [rdi + 88]
    mov r9,  [rdi + 80]
    mov r10, [rdi + 72]
    mov r11, [rdi + 208]
    mov r12, [rdi + 56]
    mov r13, [rdi + 48]
    mov r14, [rdi + 40]
    mov r15, [rdi + 32]
    mov rax, [rdi + 168]
    mov rdi, [rdi + 96]

    ; int 3
    ; jmp $
    
    o64 sysret