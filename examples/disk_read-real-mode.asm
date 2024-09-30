ORG 0
BITS 16

_start:
    jmp short start
    nop

times 33 db 0 ; to make space for bios to write bios parameter block

start:
    jmp 0x7c0:boot

boot:
    cli ; Clear Interrupts
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enable Interrupts
    call print_message_from_disk
    jmp $

print:
    mov bx, 0
.loop:
    lodsb
    cmp al, 0
    je .done
    call print_char
    jmp .loop
.done:
    ret

print_char:
    mov ah, 0eh
    int 0x10
    ret


print_message_from_disk:
    mov ah, 2
    mov al, 1
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov bx, buffer
    int 0x13
    jc handle_disk_error
    mov si, buffer
    call print
    jmp disk_read_done
handle_disk_error:
    mov si, disk_error
    call print
    jmp disk_read_done
disk_read_done:
    ret

disk_error: db "Error reading from disk...", 0
times 510-($ - $$) db 0
dw 0xAA55
buffer:
