#include "interrupts.h"
#include <stdint.h>

#include "config.h"
#include "kernel.h"
#include "task/task.h"
#include "memory/memory.h"
#include "terminal/terminal.h"
#include "io/io.h"

struct interrupt_table interrupt_table;
struct interrupt_entry interrupt_entries[512];
static ISR80H_COMMAND isr80h_commands[ZHIOS_MAX_ISR80H_COMMANDS];

extern void load_interrupts(struct interrupt_table *table);
extern void no_interrupt();
extern void int21h();
extern void int0h();
extern void enable_interrupts();
extern void isr80h();

void isr80h_register_command(int command, ISR80H_COMMAND handler)
{
    if (command < 0 || command >= ZHIOS_MAX_ISR80H_COMMANDS)
    {
        print("Comand code overflow error");
        while (1)
        {
        }
    }
    if (isr80h_commands[command])
    {
        print("Exisiting command code");
        while (1)
        {
        }
    }
    isr80h_commands[command] = handler;
}

void *isr80h_handle_command(int command, struct InterruptFrame *frame)
{
    if (command < 0 || command >= ZHIOS_MAX_ISR80H_COMMANDS)
    {
        return 0;
    }
    if (!isr80h_commands[command])
    {
        return 0;
    }
    return isr80h_commands[command](frame);
}

void divide_by_zero()
{
    print("Divide by zero error!\n");
}

void int21h_handler()
{
    print("Keybord key pressed!\n");
    outb(0x20, 0x20);
}

void no_interrupt_handler()
{
    outb(0x20, 0x20);
}

void *isr80h_handler(int command, struct InterruptFrame *frame)
{
    void *res = 0;
    kernel_page();
    task_save_current_state(frame);
    res = isr80h_handle_command(command, frame);
    current_task_page();
    return res;
}

void set_interrupt(int interrupt_num, void *address)
{
    struct interrupt_entry *entry = &interrupt_entries[interrupt_num];
    entry->offset_1 = (uint32_t)address & 0xffff;
    entry->selector = KERNEL_CODE_SELECTOR;
    entry->zero = 0x00;
    entry->type_attr = 0xEE; // 1110 1110
    entry->offset_2 = (uint32_t)address >> 16 & 0xffff;
}

void init_interrupts()
{
    memset(interrupt_entries, 0, sizeof(interrupt_entries));
    interrupt_table.limit = sizeof(interrupt_entries) - 1;
    interrupt_table.base = (uint32_t)interrupt_entries;

    for (int i = 0; i < TOTAL_INTERRUPTS; i++)
    {
        set_interrupt(i, no_interrupt);
    }

    set_interrupt(0, int0h);
    set_interrupt(0x21, int21h);
    set_interrupt(0x80, isr80h);

    load_interrupts(&interrupt_table);
}
