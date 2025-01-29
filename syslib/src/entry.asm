[BITS 32]

global _start
extern main
extern system_exit_process

section .text

_start:
    call main
    call system_exit_process
    ret
