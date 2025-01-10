#include "isr80h.h"
#include "interrupts/interrupts.h"
#include "task/task.h"
#include "terminal/terminal.h"

void *sum(struct InterruptFrame *frame)
{
    int param2 = (int)task_get_stack_item(get_current_task(), 1);
    int param1 = (int)task_get_stack_item(get_current_task(), 0);
    return (void *)(param1 + param2);
}

void *terminal_output(struct InterruptFrame *frame)
{
    void *user_space_str = task_get_stack_item(get_current_task(), 0);
    char buffer[1024];
    copy_string_from_task(get_current_task(), user_space_str, buffer, sizeof(buffer));
    print(buffer);
    return 0;
}

void register_system_commands()
{
    isr80h_register_command(SYTEM_COMMAND0_SUM, sum);
    isr80h_register_command(SYTEM_COMMAND1_PRINT, terminal_output);
}
