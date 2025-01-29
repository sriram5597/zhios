[BITS 32]

section .text

global system_start_process:function
global system_exit_process:function

system_start_process:
    push ebp
    mov ebp, esp
    mov eax, 6
    push dword [ebp + 8]
    int 0x80
    add esp, 4
    pop ebp
    ret

system_exit_process:
    push ebp
    mov ebp, esp
    mov eax, 7
    int 0x80
    pop ebp
    ret
