#include "interrupts.h"
#include<stdint.h>

#include "config.h"
#include "memory/memory.h"
#include "terminal/terminal.h"
#include "io/io.h"

struct interrupt_table interrupt_table;
struct interrupt_entry interrupt_entries[512];

extern void load_interrupts(struct interrupt_table* table);
extern void no_interrupt();
extern void int21h();
extern void int0h();
extern void enable_interrupts();

void divide_by_zero() {
    print("Divide by zero error!\n");
}

void int21h_handler() {
    print("Keybord key pressed!\n");
    outb(0x20, 0x20);
}

void no_interrupt_handler() {
    outb(0x20, 0x20);
}

void set_interrupt(int interrupt_num, void* address) {
    struct interrupt_entry* entry = &interrupt_entries[interrupt_num];
    entry -> offset_1 = (uint32_t) address & 0x0000ffff;
    entry -> selector = KERNEL_CODE_SELECTOR;
    entry -> zero = 0x00;
    entry -> type_attr = 0xEE;
    entry -> offset_2 = (uint32_t) address >> 16;
}

void init_interrupts() {
    memset(interrupt_entries, 0, sizeof(interrupt_entries));
    interrupt_table.limit = sizeof(interrupt_entries) - 1;
    interrupt_table.base = (uint32_t) interrupt_entries;

    for (int i = 0; i < TOTAL_INTERRUPTS; i++) {
        set_interrupt(i, no_interrupt);
    }

    set_interrupt(0, int0h);
    set_interrupt(0x21, int21h);

    load_interrupts(&interrupt_table);
    enable_interrupts();
}
