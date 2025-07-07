global changeRSP
section .text
changeRSP:
    pop rdx
    add rdi, rsi
    mov rsp, rdi
    push rdx
    ret
    