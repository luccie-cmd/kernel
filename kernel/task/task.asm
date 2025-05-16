global switchProc
global syscallEntry
extern kernelCR3
extern tssRSP0
extern syscallHandler
section .trampoline.text
syscallEntry:
    ; Uh yeah this could go very badly if the user decided to call syscall with RSP set to 0 so FIXME ig? (CVE )
    push rbp
    mov rbp, rsp
    mov rsp, [tssRSP0]
    push rbp
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
    call syscallHandler

    ud2
    ; pop rax
    ; pop rbx
    ; pop rcx
    ; pop rdx
    ; pop rbp
    ; pop rsi
    ; pop rdi
    ; pop r8
    ; pop r9
    ; pop r10
    ; pop r11
    ; pop r12
    ; pop r13
    ; pop r14
    ; pop r15

    ; pop rax
    ; mov cr3, rax
    ; pop rax
    ; mov rsp, rbp
    ; pop rbp

    ; o64 sysret
switchProc:
    mov rax, [rdi + 0]
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

    mov rbx, [rdi + 144]
    mov rcx, [rdi + 200]
    mov rdx, [rdi + 128]
    mov rbp, [rdi + 120]
    mov rsp, [rdi + 224]
    mov rsi, [rdi + 112]
    mov r8, [rdi + 96]
    mov r9, [rdi + 88]
    mov r10, [rdi + 80]
    mov r11, [rdi + 216]
    mov r12, [rdi + 64]
    mov r13, [rdi + 56]
    mov r14, [rdi + 48]
    mov r15, [rdi + 40]
    mov rax, [rdi + 176]
    mov rdi, [rdi + 104]

    ; int 3
    ; jmp $
    
    o64 sysret