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
    copy_string_from_task(get_current_task(), filename, buffer, sizeof(buffer));
    strcat("0:/bin/", buffer, path);
    strcat(path, ".elf", path);
    struct Process *process = 0;
    int res = process_load_and_switch(path, &process);
    if (res < 0)
    {
        goto out;
    }
    switch_task(process->task);
    restore_task(&process->task->registers);
out:
    return 0;
}
