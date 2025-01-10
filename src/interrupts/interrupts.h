#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

struct InterruptFrame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

struct interrupt_entry
{
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_2;
} __attribute__((packed));

struct interrupt_table
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef void *(*ISR80H_COMMAND)(struct InterruptFrame *frame);

void init_interrupts();
void enable_interrupts();
void isr80h_register_command(int command, ISR80H_COMMAND handler);

#endif
