#include "memory.h"
#include <stddef.h>
#include "task/task.h"
#include "task/process.h"

void *isr80h_malloc_command(struct InterruptFrame *frame)
{
    size_t size = (size_t)task_get_stack_item(get_current_task(), 0);
    return process_malloc(get_current_task()->process, size);
}

void *isr80h_free_command(struct InterruptFrame *frame)
{
    void *ptr = task_get_stack_item(get_current_task(), 0);
    process_free_allocation(get_current_task()->process, ptr);
    return 0;
}