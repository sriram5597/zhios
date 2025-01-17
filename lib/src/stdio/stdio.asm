[BITS 32]

global print:function
global system_getchar:function
global system_putchar:function

section .text
; void print(const char* message)
print:
    push ebp
    mov ebp, esp
    push dword [esp + 8]
    mov eax, 1
    int 0x80
    add esp, 4
    pop ebp
    ret

; int getchar()
system_getchar:
    push ebp
    mov eax, 3
    int 0x80
    pop ebp
    ret


system_putchar:
    push ebp
    mov ebp, esp
    push dword[ebp + 8]
    mov eax, 2
    int 0x80
    add esp, 4
    pop ebp
    ret
