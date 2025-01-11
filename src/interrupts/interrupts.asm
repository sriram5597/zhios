section .text

global load_interrupts
global no_interrupt
global int0h
global isr80h
global enable_interrupts
global disable_insterrupts
global isr_pointer_table

extern no_interrupt_handler
extern divide_by_zero
extern isr80h_handler
extern isr_handler

enable_interrupts:
    sti
    ret

disable_insterrupts:
    cli
    ret

load_interrupts:
    push ebp
    mov ebp, esp
    mov ebx, [ebp + 8]
    lidt [ebx]
    pop ebp
    ret

no_interrupt:
    pushad
    call no_interrupt_handler
    popad
    iret

int0h:
    pushad
    call divide_by_zero
    popad
    iret

isr80h:
    ; INTERRUPT FRAME START
    ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
    ; uint32_t ip
    ; uint32_t cs;
    ; uint32_t flags
    ; uint32_t sp;
    ; uint32_t ss;
    ; Pushes the general purpose registers to the stack
    pushad
    ; INTERRUPT FRAME END

    ; Push stack pointer to point to frame
    push esp

    ;Push Command
    push eax
    call isr80h_handler
    mov dword [ret_value], eax
    add esp,8

    ; Restore General purpose register for userland
    popad
    mov eax, [ret_value]
    iretd

%macro interrupt 1
    global isr%1
    isr%1:
        ; INTERRUPT FRAME START
        ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
        ; uint32_t ip
        ; uint32_t cs;
        ; uint32_t flags
        ; uint32_t sp;
        ; uint32_t ss;
        ; Pushes the general purpose registers to the stack
        pushad
        ; INTERRUPT FRAME END
        push esp
        push dword %1
        call isr_handler
        add esp, 8
        popad
        iret
%endmacro

%assign i 0
%rep 512
    interrupt i
    %assign i i+1
%endrep

section .data
ret_value: dd 0

%macro isr_array_entry 1
    dd isr%1
%endmacro

isr_pointer_table:
%assign i 0
%rep 512
    isr_array_entry i
    %assign i i+1
%endrep
