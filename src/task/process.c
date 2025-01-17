#include "process.h"
#include <stddef.h>
#include "status.h"
#include "config.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "fs/file.h"
#include "string/string.h"
#include "loaders/loader.h"

static struct Process *current_process = 0;
static struct Process *process_list[ZHIOS_MAX_PROCESS] = {0};

static void init_process(struct Process *process)
{
    memset(process, 0, sizeof(struct Process));
}

struct Process *get_current_process()
{
    return current_process;
}

int process_map_memory(struct Process *process)
{
    int res = 0;
    void *data = process->file_type == PROCESS_FILETYPE_ELF ? process->elf_file : process->ptr;
    res = loader_map_memory(process->task->page_directory, data, process->size, process->file_type);
    if (res < 0)
    {
        return res;
    }
    int page_flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE;
    res = map_page_range(process->task->page_directory, (void *)ZHIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, ZHIOS_PROGRAM_STACK_SIZE, page_flags);
    return res;
}

int get_process_free_slot()
{
    for (int i = 0; i < ZHIOS_MAX_PROCESS; i++)
    {
        if (process_list[i] == 0)
        {
            return i;
        }
    }
    return MAX_PROCESS_REACHED;
}

struct Process *get_process(int process_id)
{
    return process_list[process_id];
}

int load_process(const char *filename, struct Process **process)
{
    int process_id = get_process_free_slot();
    if (process_id == MAX_PROCESS_REACHED)
    {
        return MAX_PROCESS_REACHED;
    }
    int res = 0;
    struct Process *_process;
    void *stack_ptr = 0;

    _process = kzalloc(sizeof(struct Process));
    init_process(_process);
    void *data = 0;
    res = load_data(filename, &data, &_process->size);
    if (res <= 0)
    {
        kfree(_process);
        goto out;
    }
    _process->file_type = (enum ProcessFileType)res;
    if (_process->file_type == PROCESS_FILETYPE_ELF)
    {
        _process->elf_file = (struct ElfFile *)data;
    }
    else
    {
        _process->ptr = data;
    }
    stack_ptr = kzalloc(ZHIOS_PROGRAM_STACK_SIZE);
    if (!stack_ptr)
    {
        res = -EMEM;
        goto out;
    }
    strcpy(filename, (char *)_process->filename);
    _process->stack = stack_ptr;
    _process->id = process_id;
    _process->task = create_task(_process);
    process_map_memory(_process);
    process_list[process_id] = _process;
    *process = _process;
out:
    return res;
}

void process_switch(struct Process *process)
{
    current_process = process;
}

int process_load_and_switch(const char *filename, struct Process **process)
{
    int res = load_process(filename, process);
    if (res < 0)
    {
        return res;
    }
    process_switch(*process);
    return res;
}

void free_process(int process_id)
{
    struct Process *process = get_process(process_id);
    free_task(process->task);
    kfree(process->stack);
    kfree(process->ptr);
    kfree(process);
}

static int process_get_allocation(struct Process *process)
{
    int res = -EMEM;
    for (int i = 0; i < ZHIOS_PROCESS_MAX_ALLOCATIONS; i++)
    {
        if (process->allocations[i] == 0)
        {
            res = i;
            break;
        }
    }
    return res;
}

void *process_malloc(struct Process *process, size_t size)
{
    int ind = process_get_allocation(process);
    if (ind < 0)
    {
        return 0;
    }
    void *ptr = kzalloc(size);
    if (!ptr)
    {
        return 0;
    }
    process->allocations[ind] = ptr;
    return ptr;
}

static int process_get_ptr_allocation(struct Process *process, void *ptr)
{
    int res = -EMEM;
    for (int i = 0; i < ZHIOS_PROCESS_MAX_ALLOCATIONS; i++)
    {
        if (process->allocations[i] == ptr)
        {
            res = i;
            break;
        }
    }
    return res;
}

void process_free_allocation(struct Process *process, void *ptr)
{
    int ind = process_get_ptr_allocation(process, ptr);
    if (ind < 0)
    {
        return;
    }
    process->allocations[ind] = 0x00;
    kfree(ptr);
}