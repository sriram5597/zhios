[BITS 16]
section .text

global _start

_start:
    mov cx, 0x118
    mov di, 0x5000
    mov ax, 0x4F01
    int 0x10
    cmp ax, 0x4F
    jne vesa_err
    call set_video_mode
    pop ax
    jmp ax

set_video_mode:
    push bp
    mov bp, sp
    mov ax, 0x4F02
    mov bx, 0x4118
    int 0x10
    cmp ax, 0x4F
    jne vesa_err
    pop bp
    ret

vesa_err:
    mov al, 'F'
    mov ah, 0x0E
    int 0x10
    hlt

times 512 - ($ - $$) db 0

section .bss
vesa_info_buffer resb 512
