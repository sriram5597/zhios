[BITS 32]
global _start
global kernel_registers

extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000
    mov esp, ebp

    ; Remap PCI interrupts to start from 0x20
    mov al, 00010001b
    out 0x20, al ; Tell master PIC
    mov al ,0x20 ; interrupt 0x20 is where master ISR starts
    out 0x21, al 

    mov al, 0x04 ; ICW3
    out 0x21, al

    mov al, 00000001b
    out 0x21, al
    ; End remap

    call kernel_main
    jmp $

kernel_registers:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret

times 512 - ($ - $$) db 0
