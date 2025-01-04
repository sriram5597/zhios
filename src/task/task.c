#include "task.h"
#include "status.h"
#include "config.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"

struct Task *current_task = 0;
struct Task *head_task = 0;
struct Task *tail_task = 0;

static int init_task(struct Task *, struct Process *);

struct Task *create_task(struct Process *process)
{
    int res = 0;
    struct Task *task = kzalloc(sizeof(struct Task));
    init_task(task, process);
    if (task == 0)
    {
        res = -EIO;
        goto out;
    }
    if (head_task == 0)
    {
        head_task = task;
        tail_task = task;
        goto out;
    }
    tail_task->next = task;
    task->prev = tail_task;
    tail_task = task;
out:
    if (res < 0)
    {
        free_task(task);
        return 0;
    }
    return task;
}

struct Task *get_next_task()
{
    if (!current_task->next)
    {
        return head_task;
    }
    return current_task->next;
}

static void remove_from_task_list(struct Task *task)
{
    if (task->prev)
    {
        task->prev->next = task->next;
    }
    if (task == head_task)
    {
        head_task = task->next;
    }
    if (task == tail_task)
    {
        tail_task = task->prev;
    }
    if (task == current_task)
    {
        current_task = get_next_task();
    }
}

struct Task *get_current_task()
{
    return current_task;
}

static int init_task(struct Task *task, struct Process *process)
{
    memset(task, 0, sizeof(struct Task));
    task->page_directory = paging_new_chunk(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (!task->page_directory)
    {
        return -EIO;
    }
    task->process = process;
    task->registers.ip = ZHIOS_PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.esp = ZHIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    return 0;
}

int free_task(struct Task *task)
{
    free_page(task->page_directory);
    remove_from_task_list(task);
    return 0;
}