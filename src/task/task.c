#include "task.h"
#include "process.h"
#include "status.h"
#include "config.h"
#include "kernel.h"
#include "loaders/loader.h"
#include "loaders/formats/elf.h"
#include "loaders/formats/elfloader.h"
#include "string.h"
#include "memory/heap/kheap.h"
#include "terminal/terminal.h"
#include "memory.h"
#include "memory/paging/paging.h"
#include "interrupts/interrupts.h"

static struct Task *current_task = 0;
static struct Task *head_task = 0;
static struct Task *tail_task = 0;

static int init_task(struct Task *, struct Process *);

extern void restore_task(struct Registers *registers);

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
        current_task = task;
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

void init_stack(struct Task *task)
{
    if (task->process->parameters->count > 0)
    {
        int *arg_ptr = (int *)(ZHIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START - sizeof(int));
        print("Argument: ");
        print(task->process->parameters->command_arguments[0]);
        print("\n");
        *arg_ptr = task->process->parameters->count;
    }
}

int switch_task(struct Task *task)
{
    current_task = task;
    paging_switch(task->page_directory);
    // init_stack(task);
    return 0;
}

int current_task_page()
{
    user_registers();
    switch_task(current_task);
    return 0;
}

int switch_to_task_page(struct Task *task)
{
    user_registers();
    paging_switch(task->page_directory);
    return 0;
}

void run_first_task()
{
    if (!head_task)
    {
        print("No tasks to execute..");
        return;
    }
    switch_task(head_task);
    restore_task(&head_task->registers);
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
    if (process->file_type == PROCESS_FILETYPE_ELF)
    {
        task->registers.ip = elf_get_header(process->elf_file)->e_entry;
    }
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.cs = USER_CODE_SEGMENT;
    task->registers.esp = ZHIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    task->registers.ebp = ZHIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    return 0;
}

int free_task(struct Task *task)
{
    free_page(task->page_directory);
    remove_from_task_list(task);
    return 0;
}

void save_state(struct Task *task, struct InterruptFrame *frame)
{
    task->registers.edi = frame->edi;
    task->registers.esi = frame->esi;
    task->registers.ebx = frame->ebx;
    task->registers.edx = frame->edx;
    task->registers.ecx = frame->ecx;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.esp = frame->esp;
    task->registers.cs = frame->cs;
    task->registers.ip = frame->ip;
    task->registers.ss = frame->ss;
    task->registers.flags = frame->flags;
}

int copy_string_from_task(struct Task *task, void *virtual, void *phy_address, int max)
{
    if (max >= PAGING_PAGE_SIZE)
    {
        return -EINARG;
    }
    int res = 0;
    char *tmp = kzalloc(max);
    if (!tmp)
    {
        return -EMEM;
    }
    uint32_t old_entry = get_page(task->page_directory->directory_entry, tmp);
    map_page(task->page_directory, tmp, tmp, PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(task->page_directory);
    strncpy(virtual, tmp, max);
    kernel_page();
    res = set_page(task->page_directory->directory_entry, tmp, old_entry);
    if (res < 0)
    {
        goto out;
    }
    strncpy(tmp, phy_address, max);
out:
    if (tmp)
    {
        kfree(tmp);
    }
    return res;
}

void task_save_current_state(struct InterruptFrame *frame)
{
    if (!get_current_task())
    {
        print("No active tasks..");
        while (1)
        {
        }
    }
    struct Task *task = get_current_task();
    save_state(task, frame);
}

void *task_get_stack_item(struct Task *task, int index)
{
    void *result = 0;
    uint32_t *st_ptr = (uint32_t *)task->registers.esp;
    switch_to_task_page(task);
    result = (void *)st_ptr[index];
    kernel_page();
    return result;
}
