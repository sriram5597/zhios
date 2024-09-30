#include "kernel.h"
#include<stdint.h>
#include<stddef.h>

#include "terminal/terminal.h"
#include "interrupts/interrupts.h"

void kernel_main() {
    init_terminal();
    init_interrupts();
    print("Hello World!\n");
}
