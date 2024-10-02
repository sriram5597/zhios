section .asm

global load_interrupts
global int21h
global no_interrupt
global int0h
global enable_interrupts
global disable_insterrupts

extern int21h_handler
extern no_interrupt_handler
extern divide_by_zero

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

int21h:
    cli
    pushad
    call int21h_handler
    popad
    sti
    iret

no_interrupt:
    cli
    pushad
    call no_interrupt_handler
    popad
    sti
    iret

int0h:
    cli
    pushad
    call divide_by_zero
    popad
    sti
    iret

    xor ax, ax
    div ax
    ret
