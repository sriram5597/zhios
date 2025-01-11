#include "io.h"
#include "terminal/terminal.h"
#include "task/task.h"
#include "keyboard/keyboard.h"
#include "interrupts/interrupts.h"

void *terminal_output(struct InterruptFrame *frame)
{
    void *user_space_str = task_get_stack_item(get_current_task(), 0);
    char buffer[1024];
    copy_string_from_task(get_current_task(), user_space_str, buffer, sizeof(buffer));
    print(buffer);
    return 0;
}

void *putchar(struct InterruptFrame *frame)
{
    char c = (char)((int)task_get_stack_item(get_current_task(), 0));
    terminal_writechar(c);
    return 0;
}

void *get_key_from_keyboard(struct InterruptFrame *frame)
{
    char c = keyboard_pop();
    return (void *)((int)c);
}