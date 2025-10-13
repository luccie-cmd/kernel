global switchProc
global syscallEntry
global jumpSignal
extern kernelCR3
extern tempValue
extern syscallHandler
section .trampoline.data
global syscallSpinlock
syscallSpinlock: db 0
tempGdt:
    .limit: dw 0
    .base: dq 0
section .trampoline.text
syscallEntry:
.loopSpinlock:
    mov     r10b, 0x1
    xchg BYTE [syscallSpinlock], r10b
    test    r10b, r10b
    jne .loopSpinlock
    mov [tempValue], rsp
    swapgs
    mov r10, gs:0x10
    mov cr3, r10
    mov rsp, gs:0x18
    push rax

    mov eax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push r15
    push r14
    push r13
    push r12
    push r11
    push r9
    push r8
    push rdi
    push rsi
    push rbp
    push rdx
    push rcx
    push rbx

    lea rdi, [rsp]
    mov BYTE [syscallSpinlock], 0
    jmp syscallHandler
    
switchProc:
    mov cr3, rsi
    mfence
    jmp .flush
.flush:
    mov r15, rdx

    mov ecx, 0xC0000080
	rdmsr
    or eax, 1
	wrmsr

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rdx, r15
    mov ecx, 0xC0000100
    mov eax, edx
    shr rdx, 32
    wrmsr

    mov rax, [rdi + 168]
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
    mov rdi, [rdi + 96]

    ; int3
    ; jmp $
    
    o64 sysret

jumpSignal:
    mov cr3, rsi
    mfence
    jmp .flush
.flush:
    push 0x23
    lea rax, [rel .reload_CS]
    push rax
    retfq
.reload_CS:
    mov ax, 0x1B
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    call rdi
    mov rax, [kernelCR3]
    mov cr3, rax