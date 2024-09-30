section .asm

global load_interrupts

load_interrupts:
    push ebp
    mov ebp, esp
    mov ebx, [ebp + 8]
    lidt [ebx]
    pop ebp
    ret
