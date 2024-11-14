ORG 0x7C00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

jmp short start
nop

; FAT16 Header
OEMIdentifier     db 'ZHIOS   '
BytesPerSector    dw 512
SectorsPerCluster db 128
ReservedSectors   dw 200
FATCopies         db 2
RootDirEntries    dw 64
NumSectors        dw 0x00
MediaType         db 0xF8
SectorsPerFat     dw 256
SectorsPerTrack   dw 0x20
NumberOfHeads     dw 0x40
HiddenSectors     dd 0x00
LargeSectors      dd 0x773594

; Extended BPS
DriveNumber       db 0x80
WinNtFlags        db 0x00
Signature         db 0x29
VolumeID          dd 0xD105
VolumeLabel       db 'Zhios Boot '
FilesystemType    db 'FAT16   '

start:
    jmp 0x00:boot

boot:
    cli ; Clear Interrupts
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti ; Enable Interrupts

.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32

; Creating GDT Entries
gdt_start:

gdt_null:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xFFFF ; Segment limit first 0-15 bits
    dw 0x0 ; Base first 0-15 bits
    db 0 ; Limit 16-23 bits
    db 0x9A ; Access Byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0        ; Base 24-31 bits


gdt_data:
    dw 0xFFFF ; Segment limit first 0-15 bits
    dw 0x0 ; Base first 0-15 bits
    db 0 ; Limit 16-23 bits
    db 0x92 ; Access Byte
    db 11001111b ; High 4 bit flags and the low 4 bit flags
    db 0        ; Base 24-31 bits


gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[BITS 32]
load32:
    mov eax, 1
    mov ecx, 100
    mov edi, 0x0100000
    call lba_read
    jmp CODE_SEG:0x0100000

lba_read:

    ; Port to send drive and 24-27 bit of LBA
    mov ebx, eax
    shr eax, 24
    or eax, 0xE0 ; Select master drive
    mov edx, 0x01F6
    out dx, al

    ; Port to send number of sectors to read
    mov edx, 0x01F2
    mov eax, ecx
    out dx, al

    ; Port to send 0-7 bit of LBA
    mov edx, 0x01F3
    mov eax, ebx
    out dx, al

    ; Port to send 8-15 bit of LBA
    mov edx, 0x01F4
    mov eax, ebx
    shr eax, 8
    out dx, al

    ; Port to send 16-23 bit of LBA
    mov edx, 0x01F5
    mov eax, ebx
    shr eax, 16
    out dx, al

    ; Command Port
    mov edx, 0x01F7
    mov al, 0x20
    out dx, al

.next_sector:
    push ecx

.retry:
    mov dx, 0x01F7
    in al, dx
    test al, 8
    jz .retry

    mov ecx, 256
    mov dx, 0x1f0
    rep insw
    pop ecx
    loop .next_sector
    ret

times 510-($ - $$) db 0
dw 0xAA55
