#include "process.h"
#include "status.h"
#include "config.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "fs/file.h"
#include "string/string.h"

static struct Process *current_process = 0;
static struct Process *process_list[ZHIOS_MAX_PROCESS] = {0};

static void init_process(struct Process *process)
{
    memset(process, 0, sizeof(struct Process));
}

static int load_binary(const char *filename, struct Process *process)
{
    int res = 0;
    int fd = fopen(filename, "r");
    if (fd == 0)
    {
        res = -EIO;
        goto out;
    }
    struct FileStat stat;
    res = fstat(fd, &stat);
    if (res < 0)
    {
        goto out;
    }
    void *program_data = kzalloc(stat.size);
    if (!program_data)
    {
        res = -EMEM;
        goto out;
    }
    if (fread(fd, program_data, stat.size) < 0)
    {
        res = -EIO;
        goto out;
    }
    process->ptr = program_data;
    process->size = stat.size;
out:
    fclose(fd);
    return res;
}

static int load_data(const char *filename, struct Process *process)
{
    int res = 0;
    res = load_binary(filename, process);
    return res;
}

struct Process *get_current_process()
{
    return current_process;
}

int process_map_binary(struct Process *process)
{
    int res = 0;
    int flags = PAGING_IS_PRESENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL;
    int page_count = (align_to_paging_address(process->ptr + process->size) - process->ptr) / PAGING_PAGE_SIZE;
    res = map_page_range(process->task->page_directory, (void *)ZHIOS_PROGRAM_VIRTUAL_ADDRESS, process->ptr, page_count, flags);
    return res;
}

int process_map_memory(struct Process *process)
{
    int res = 0;
    res = process_map_binary(process);
    if (res < 0)
    {
        return res;
    }
    int page_count = (align_to_paging_address(process->stack + ZHIOS_PROGRAM_STACK_SIZE) - process->stack) / PAGING_PAGE_SIZE;
    int page_flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE;
    res = map_page_range(process->task->page_directory, (void *)ZHIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, page_count, page_flags);
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
    res = load_data(filename, _process);
    if (res < 0)
    {
        kfree(_process);
        goto out;
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
