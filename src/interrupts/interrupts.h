#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include<stdint.h>

struct interrupt_entry {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_2;
} __attribute__((packed));

struct interrupt_table {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void init_interrupts();

#endif
