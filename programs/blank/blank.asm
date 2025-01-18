[BITS 32]

section .text

global _start

_start:
    mov eax, message
    push eax
    mov eax, 1
    int 0x80
    add esp, 4
_loop:
    call getKey
    cmp eax, 0x1B
    je out
    push eax
    mov eax, 2
    int 0x80
    add esp, 4
    jmp _loop
    ret

getKey:
    mov eax, 3
    int 0x80
    cmp eax, 0x00
    je getKey
    ret

out:
    jmp $

section .data
    message: db "Loaded ELF File....", 0