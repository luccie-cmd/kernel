# 0 "kernel/hal/arch/x64/init.asm"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "kernel/hal/arch/x64/init.asm"
global initX64
; extern abort
; extern puts
; extern _ZN3dbg6printfEPKcz
section .text
initX64:
    mov rax, cr0
    and rax, ~(1 << 2)
    or rax, (1 << 1)
    mov cr0, rax
    mov rax, cr4
    or rax, (1 << 9) | (1 << 10)
    mov cr4, rax
    ret
; push rax
; push rbx
; push rcx
; push rdx
; push rdi
; push rsi
; mov rax, 1
; cpuid
; test rdx, (1 << 25)
; jz .no_sse
; .sse:
; test rdx, (1 << 26)
; jz .no_sse2
; .sse2:
; test rcx, (1 << 0)
; jz .no_sse3
; .sse3:
; test rcx, (1 << 9)
; jz .no_ssse3
; .ssse3:
; test rcx, (1 << 19)
; jz .no_sse41
; .sse41:
; test rcx, (1 << 20)
; jz .no_sse42
; .sse42:
; test rcx, (1 << 26)
; jz .no_xsave
; mov rax, cr4
; or rax, (1 << 18)
; mov cr4, rax
; test rcx, (1 << 28)
; jz .no_avx
; .avx:
; mov rax, 7
; xor rcx, rcx
; cpuid
; test rbx, (1 << 5)
; jz .no_avx2
; .avx2:
; test rbx, (1 << 16)
; jz .no_avx512f
; .avx512f:
; test rbx, (1 << 17)
; jz .no_avx512dq
; .avx512dq:
; test rbx, (1 << 21)
; jz .no_avx512ifma
; .avx512ifma:
; test rbx, (1 << 26)
; jz .no_avx512pf
; .avx512pf:
; test rbx, (1 << 27)
; jz .no_avx512er
; .avx512er:
; test rbx, (1 << 28)
; jz .no_avx512cd
; .avx512cd:
; test rbx, (1 << 30)
; jz .no_avx512bw
; .avx512bw:
; test rbx, QWORD (1 << 31)
; jz .no_avx512vl
; .avx512vl:
; test rcx, (1 << 1)
; jz .no_avx512vbmi
; .avx512vbmi:
; test rcx, (1 << 11)
; jz .no_avx512vnni
; .avx512vnni:
; test rbx, (1 << 3)
; jz .no_bmi
; .bmi:
; test rbx, (1 << 8)
; jz .no_bmi2
; .bmi2:
; jmp .setExts
; .no_sse:
; mov rdi, str0
; mov BYTE [sse], 1
; call .ext_not_supported
; call abort
; .no_sse2:
; mov rdi, str1
; mov BYTE [sse2], 1
; call .ext_not_supported
; call abort
; .no_sse3:
; mov rdi, str2
; mov BYTE [sse3], 1
; call .ext_not_supported
; jmp .sse3
; .no_ssse3:
; mov rdi, str3
; mov BYTE [ssse3], 1
; call .ext_not_supported
; jmp .ssse3
; .no_sse41:
; mov rdi, str4
; mov BYTE [sse41], 1
; call .ext_not_supported
; jmp .sse41
; .no_sse42:
; mov rdi, str5
; mov BYTE [sse42], 1
; call .ext_not_supported
; jmp .sse42
; .no_xsave:
; mov rdi, str20
; call .ext_not_supported
; jmp .bmi
; .no_avx:
; mov rdi, str6
; mov BYTE [avx], 1
; call .ext_not_supported
; jmp .avx
; .no_avx2:
; mov rdi, str7
; mov BYTE [avx2], 1
; call .ext_not_supported
; jmp .avx2
; .no_bmi:
; mov rdi, str8
; mov BYTE [bmi], 1
; call .ext_not_supported
; jmp .bmi
; .no_bmi2:
; mov rdi, str9
; mov BYTE [bmi2], 1
; call .ext_not_supported
; jmp .bmi2
; .no_avx512f:
; mov rdi, str10
; mov BYTE [avx512f], 1
; call .ext_not_supported
; jmp .bmi
; .no_avx512dq:
; mov rdi, str11
; mov BYTE [avx512dq], 1
; call .ext_not_supported
; jmp .avx512dq
; .no_avx512ifma:
; mov rdi, str12
; mov BYTE [avx512ifma], 1
; call .ext_not_supported
; jmp .avx512ifma
; .no_avx512pf:
; mov rdi, str13
; mov BYTE [avx512pf], 1
; call .ext_not_supported
; jmp .avx512pf
; .no_avx512er:
; mov rdi, str14
; mov BYTE [avx512er], 1
; call .ext_not_supported
; jmp .avx512er
; .no_avx512cd:
; mov rdi, str15
; mov BYTE [avx512cd], 1
; call .ext_not_supported
; jmp .avx512cd
; .no_avx512bw:
; mov rdi, str16
; mov BYTE [avx512bw], 1
; call .ext_not_supported
; jmp .avx512bw
; .no_avx512vl:
; mov rdi, str17
; mov BYTE [avx512vl], 1
; call .ext_not_supported
; jmp .avx512vl
; .no_avx512vbmi:
; mov rdi, str18
; mov BYTE [avx512vbmi], 1
; call .ext_not_supported
; jmp .avx512vbmi
; .no_avx512vnni:
; mov rdi, str19
; mov BYTE [avx512vnni], 1
; call .ext_not_supported
; jmp .avx512vnni
; .ext_not_supported:
; push rax
; push rcx
; push rdx
; push rsi
; push rdi
; push r8
; push r9
; push r10
; push r11
; mov rbp, rsp
; call puts
; mov rsp, rbp
; pop r11
; pop r10
; pop r9
; pop r8
; pop rdi
; pop rsi
; pop rdx
; pop rcx
; pop rax
; ret
; .setExts:
; mov rdx, 1
; cmp BYTE [sse], 1
; cmove rax, rdx
; cmp BYTE [sse2], 1
; cmove rax, rdx
; test rax, rax
; jne .noSseEnable
; mov rax, cr0
; and rax, ~(1 << 2)
; or rax, (1 << 1)
; mov cr0, rax
; mov rax, cr4
; or rax, (1 << 9) | (1 << 10)
; mov cr4, rax
; mov rdi, str21
; push rax
; push rcx
; push rdx
; push rsi
; push rdi
; push r8
; push r9
; push r10
; push r11
; mov rbp, rsp
; call puts
; mov rsp, rbp
; pop r11
; pop r10
; pop r9
; pop r8
; pop rdi
; pop rsi
; pop rdx
; pop rcx
; pop rax
; mov BYTE [sseEnabled], 1
; .noSseEnable:
; mov rdx, 1
; cmp BYTE [sseEnabled], 0
; cmove rax, rdx
; cmp BYTE [sse3], 1
; cmove rax, rdx
; cmp BYTE [ssse3], 1
; cmove rax, rdx
; test rax, rax
; jne .noAvxEnable
; xor rcx, rcx
; xgetbv
; or rax, (1 << 1)
; xsetbv
; mov rdi, str22
; push rax
; push rcx
; push rdx
; push rsi
; push rdi
; push r8
; push r9
; push r10
; push r11
; mov rbp, rsp
; call puts
; mov rsp, rbp
; pop r11
; pop r10
; pop r9
; pop r8
; pop rdi
; pop rsi
; pop rdx
; pop rcx
; pop rax
; mov BYTE [avxEnabled], 1
; .noAvxEnable:
; mov rdx, 1
; cmp BYTE [avxEnabled], 0
; cmove rax, rdx
; cmp BYTE [sse41], 1
; cmove rax, rdx
; cmp BYTE [sse42], 1
; cmove rax, rdx
; test rax, rax
; jne .noAvx2Enable
; xor rcx, rcx
; xgetbv
; or rax, (1 << 2)
; xsetbv
; mov rdi, str23
; push rax
; push rcx
; push rdx
; push rsi
; push rdi
; push r8
; push r9
; push r10
; push r11
; mov rbp, rsp
; call puts
; mov rsp, rbp
; pop r11
; pop r10
; pop r9
; pop r8
; pop rdi
; pop rsi
; pop rdx
; pop rcx
; pop rax
; mov BYTE [avx2Enabled], 1
; .noAvx2Enable:
; mov rdx, 1
; cmp BYTE [avx2Enabled], 0
; cmove rax, rdx
; cmp BYTE [avx512f], 1
; cmove rax, rdx
; cmp BYTE [avx512dq], 1
; cmove rax, rdx
; cmp BYTE [avx512ifma], 1
; cmove rax, rdx
; cmp BYTE [avx512pf], 1
; cmove rax, rdx
; cmp BYTE [avx512er], 1
; cmove rax, rdx
; cmp BYTE [avx512cd], 1
; cmove rax, rdx
; cmp BYTE [avx512bw], 1
; cmove rax, rdx
; cmp BYTE [avx512vl], 1
; cmove rax, rdx
; cmp BYTE [avx512vbmi], 1
; cmove rax, rdx
; cmp BYTE [avx512vnni], 1
; cmove rax, rdx
; test rax, rax
; jne .noAvx512Enable
; xor rcx, rcx
; xgetbv
; or rax, (1 << 5) | (1 << 6) | (1 << 7)
; xsetbv
; mov rdi, str24
; push rax
; push rcx
; push rdx
; push rsi
; push rdi
; push r8
; push r9
; push r10
; push r11
; mov rbp, rsp
; call puts
; mov rsp, rbp
; pop r11
; pop r10
; pop r9
; pop r8
; pop rdi
; pop rsi
; pop rdx
; pop rcx
; pop rax
; mov BYTE [avx512Enabled], 1
; .noAvx512Enable:
; xor rax, rax
; mov rdx, 1
; cmp BYTE [bmi], 0
; cmove rax, rdx
; mov [bmi1Useable], rax
; test rax, rax
; je .noBmi1Useable
; mov rdi, str25
; push rax
; push rcx
; push rdx
; push rsi
; push rdi
; push r8
; push r9
; push r10
; push r11
; mov rbp, rsp
; call puts
; mov rsp, rbp
; pop r11
; pop r10
; pop r9
; pop r8
; pop rdi
; pop rsi
; pop rdx
; pop rcx
; pop rax
; .noBmi1Useable:
; xor rax, rax
; mov rdx, 1
; cmp BYTE [bmi2], 0
; cmove rax, rdx
; mov [bmi2Useable], rax
; test rax, rax
; je .noBmi2Useable
; mov rdi, str26
; push rax
; push rcx
; push rdx
; push rsi
; push rdi
; push r8
; push r9
; push r10
; push r11
; mov rbp, rsp
; call puts
; mov rsp, rbp
; pop r11
; pop r10
; pop r9
; pop r8
; pop rdi
; pop rsi
; pop rdx
; pop rcx
; pop rax
; .noBmi2Useable:
; pop rsi
; pop rdi
; pop rdx
; pop rcx
; pop rbx
; pop rax
    ret

; section .rodata
; str0: db "INFO: SSE isn't present", 0x0a, 0
; str1: db "INFO: SSE2 isn't present", 0x0a, 0
; str2: db "INFO: SSE3 isn't present", 0x0a, 0
; str3: db "INFO: SSSE3 isn't present", 0x0a, 0
; str4: db "INFO: SSE4.1 isn't present", 0x0a, 0
; str5: db "INFO: SSE4.2 isn't present", 0x0a, 0
; str6: db "INFO: AVX isn't present", 0x0a, 0
; str7: db "INFO: AVX2 isn't present", 0x0a, 0
; str8: db "INFO: BMI1 isn't present", 0x0a, 0
; str9: db "INFO: BMI2 isn't present", 0x0a, 0
; str10: db "INFO: AVX512F isn't present, skipping rest of AVX512", 0x0a, 0
; str11: db "INFO: AVX512DQ isn't present", 0x0a, 0
; str12: db "INFO: AVX512IFMA isn't present", 0x0a, 0
; str13: db "INFO: AVX512PF isn't present", 0x0a, 0
; str14: db "INFO: AVX512ER isn't present", 0x0a, 0
; str15: db "INFO: AVX512CD isn't present", 0x0a, 0
; str16: db "INFO: AVX512BW isn't present", 0x0a, 0
; str17: db "INFO: AVX512VL isn't present", 0x0a, 0
; str18: db "INFO: AVX512VBMI isn't present", 0x0a, 0
; str19: db "INFO: AVX512VNNI isn't present", 0x0a, 0
; str20: db "INFO: XSAVE isn't present, skipping AVX, AVX2 and AVX512"
; str21: db "INFO: SSE enabled", 0x0a, 0
; str22: db "INFO: AVX enabled", 0x0a, 0
; str23: db "INFO: AVX2 enabled", 0x0a, 0
; str24: db "INFO: AVX512 enabled", 0x0a, 0
; str25: db "INFO: BMI1 is useable", 0x0a, 0
; str26: db "INFO: BMI2 is useable", 0x0a, 0

; section .bss
; sseEnabled: resb 1
; avxEnabled: resb 1
; avx2Enabled: resb 1
; avx512Enabled: resb 1
; bmi1Useable: resb 1
; bmi2Useable: resb 1
; sse: resb 1
; sse2: resb 1
; sse3: resb 1
; ssse3: resb 1
; sse41: resb 1
; sse42: resb 1
; bmi: resb 1
; bmi2: resb 1
; avx: resb 1
; avx2: resb 1
; avx512f: resb 1
; avx512dq: resb 1
; avx512ifma: resb 1
; avx512pf: resb 1
; avx512er: resb 1
; avx512cd: resb 1
; avx512bw: resb 1
; avx512vl: resb 1
; avx512vbmi: resb 1
; avx512vnni: resb 1
