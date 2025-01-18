#include "isr80h.h"
#include "interrupts/interrupts.h"
#include "terminal/terminal.h"
#include "task/task.h"
#include "memory.h"
#include "io.h"
#include "process.h"

void *sum(struct InterruptFrame *frame)
{
    int param2 = (int)task_get_stack_item(get_current_task(), 1);
    int param1 = (int)task_get_stack_item(get_current_task(), 0);
    return (void *)(param1 + param2);
}

void register_system_commands()
{
    isr80h_register_command(SYTEM_COMMAND0_SUM, sum);
    isr80h_register_command(SYTEM_COMMAND1_PRINT, terminal_output);
    isr80h_register_command(SYSTEM_COMMAND2_PUTCHAR, putchar);
    isr80h_register_command(SYSTEM_COMMAND3_GETKEY, get_key_from_keyboard);
    isr80h_register_command(SYSTEM_COMMAND4_MALLOC, isr80h_malloc_command);
    isr80h_register_command(SYSTEM_COMMAND5_FREE, isr80h_free_command);
    isr80h_register_command(SYSTEM_COMMAND6_START_PROCESS, isr80h_load_process_command);
}
