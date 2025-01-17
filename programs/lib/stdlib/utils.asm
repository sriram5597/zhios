[BITS 32]

global print:function
global getkey:function

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

; int getkey()
getkey:
    push ebp
    mov eax, 3
    int 0x80
    pop ebp
    ret
