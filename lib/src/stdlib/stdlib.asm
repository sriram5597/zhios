[BITS 32]

section .text

global system_malloc:function
global system_free:function

system_malloc:
    push ebp
    mov ebp, esp
    push dword[ebp+8]
    mov eax, 4
    int 0x80
    add esp, 4
    pop ebp
    ret

system_free:
    push ebp
    mov ebp, esp
    push dword[ebp + 8]
    mov eax, 5
    int 0x80
    add esp, 4
    pop ebp
    ret
