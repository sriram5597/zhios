[BITS 32]
section .text

global restore_task
global user_registers
global restore_general_purpose_registers


; void task_return(struct registers* regs);
restore_task:
    mov ebp, esp
    ; PUSH THE DATA SEGMENT (SS WILL BE FINE)
    ; PUSH THE STACK ADDRESS
    ; PUSH THE FLAGS
    ; PUSH THE CODE SEGMENT
    ; PUSH IP
    mov ebx, [ebp + 4]

    mov ebp, [ebx + 8]
    ; push data/stack selector
    push dword [ebx + 44]

    ; push stack pointer
    push dword[ebx + 40]

    ; push flags
    pushf
    pop eax
    or eax, 0x200
    push eax

    ; push code segment
    push dword [ebx + 32]

    ;Push IP to execute
    push dword [ebx + 28]
    
    ; Setup some segment registers
    mov ax, [ebx + 44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov edi, [ebx]
    mov esi, [ebx + 4]
    mov edx, [ebx + 16]
    mov ecx, [ebx + 20]
    mov eax, [ebx + 24]
    mov ebx, [ebx + 12]

    iretd

; void user_registers()
user_registers:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret
