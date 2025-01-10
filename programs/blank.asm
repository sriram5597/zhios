[BITS 32]

section .text

global _start

_start:
    push message
    mov eax, 1
    int 0x80
    add esp, 4
    jmp $

.data
    message: db 'Loaded user program', 0