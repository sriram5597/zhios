#include "interrupts.h"
#include<stdint.h>

#include "config.h"
#include "memory/memory.h"
#include "terminal/terminal.h"

struct interrupt_table interrupt_table;
struct interrupt_entry interrupt_entries[512];

void divide_by_zero() {
    print("Divide by zero error!\n");
}

void set_interrupt(int interrupt_num, void* address) {
    struct interrupt_entry* entry = &interrupt_entries[interrupt_num];
    entry -> offset_1 = (uint32_t) address & 0x0000ffff;
    entry -> selector = KERNEL_CODE_SELECTOR;
    entry -> zero = 0x00;
    entry -> type_attr = 0xEE;
    entry -> offset_2 = (uint32_t) address >> 16;
}

extern void load_interrupts(struct interrupt_table* table);

void init_interrupts() {
    memset(interrupt_entries, 0, sizeof(interrupt_entries));
    interrupt_table.limit = sizeof(interrupt_entries) - 1;
    interrupt_table.base = (uint32_t) interrupt_entries;

    set_interrupt(0, divide_by_zero);

    load_interrupts(&interrupt_table);
}
