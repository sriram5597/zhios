#include "process.h"
#include "string.h"
#include "config.h"
#include "task/task.h"
#include "task/process.h"

void *isr80h_load_process_command(struct InterruptFrame *frame)
{
    void *filename = task_get_stack_item(get_current_task(), 0);
    char buffer[ZHIOS_MAX_PATH];
    char path[ZHIOS_MAX_PATH];
    char copy[ZHIOS_MAX_PATH];
    copy_string_from_task(get_current_task(), filename, buffer, sizeof(buffer));
    strcpy(buffer, copy);
    char *arguments[ZHIOS_MAX_PROCESS_ARGUMENTS];
    str_token(copy, arguments, ' ');
    strcat("0:/bin/", arguments[0], path);
    strcat(path, ".elf", path);
    struct Process *process = 0;
    int res = process_load_with_args(path, &process, buffer);
    if (res < 0)
    {
        goto out;
    }
    switch_task(process->task);
    restore_task(&process->task->registers);
out:
    return 0;
}
