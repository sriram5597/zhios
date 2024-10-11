#include "kernel.h"
#include<stdint.h>
#include<stddef.h>

#include "terminal/terminal.h"
#include "interrupts/interrupts.h"
#include "memory/heap/kheap.h"

void kernel_main() {
    init_terminal();
    init_interrupts();
    kheap_init();
    print("Hello World!\n");
}
