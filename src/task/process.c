#include "process.h"
#include <stddef.h>
#include "status.h"
#include "config.h"
#include "memory.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "fs/file.h"
#include "string.h"
#include "loaders/loader.h"
#include "terminal/terminal.h"

static struct Process *current_process = 0;
static struct Process *process_list[ZHIOS_MAX_PROCESS] = {0};

static void process_map_args_to_stack(struct Process *process);

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
    void *data = process->file_type == PROCESS_FILETYPE_ELF ? process->program_data.elf_file : process->program_data.elf_file;
    res = loader_map_memory(process->task->page_directory, data, process->size, process->file_type);
    if (res < 0)
    {
        return res;
    }
    int page_flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE;
    res = map_page_range(process->task->page_directory, (void *)ZHIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, ZHIOS_PROGRAM_STACK_SIZE, page_flags);
    if (process->parameters)
    {
        process_map_args_to_stack(process);
    }
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

void process_init_arguments(struct Process *process, char *args)
{
    struct ProcessParameters *params = kzalloc(sizeof(struct ProcessParameters));
    char *arguments = kzalloc(512);
    char *args_list[ZHIOS_MAX_PROCESS_ARGUMENTS];
    int args_count = str_token(args, args_list, ' ');
    for (int i = 0; i < args_count; i++)
    {
        strcpy(args_list[i], arguments);
        params->command_arguments[i] = arguments;
        arguments += strlen(args_list[i]) + 1;
    }
    params->count = args_count;
    process->parameters = params;
}

static void process_map_args_to_stack(struct Process *process)
{
    struct ProcessParameters *params = process->parameters;
    uint32_t current_addr = ZHIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    for (int i = 0; i < params->count; i++)
    {
        current_addr = current_addr - strlen(params->command_arguments[i]) - 1;
        char *arg = (char *)(paging_get_physical_address(process->task->page_directory, (void *)current_addr));
        memcpy(params->command_arguments[i], arg, strlen(params->command_arguments[i]) + 1);
    }
    uint32_t virt_arguments = current_addr - (params->count + 1) * sizeof(uint32_t);
    uint32_t virt_argv = virt_arguments - sizeof(uint32_t);
    uint32_t virt_argc = virt_argv - sizeof(uint32_t);
    uint32_t *argc = (uint32_t *)(paging_get_physical_address(process->task->page_directory, (void *)virt_argc));
    uint32_t *argv = (uint32_t *)(paging_get_physical_address(process->task->page_directory, (void *)virt_argv));
    uint32_t *args_ptr = (uint32_t *)(paging_get_physical_address(process->task->page_directory, (void *)virt_arguments));
    *argc = params->count;
    *argv = virt_arguments;
    uint32_t arg_start = ZHIOS_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
    for (int i = 0; i < params->count; i++)
    {
        arg_start = arg_start - strlen(params->command_arguments[i]) - 1;
        args_ptr[i] = arg_start;
    }
    args_ptr[params->count] = 0;
    params->start_address = virt_argc;
    process->task->registers.esp = params->start_address;
    process->task->registers.ebp = params->start_address;
}

int load_process(const char *filename, struct Process **process, char *arguments)
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
        _process->program_data.elf_file = (struct ElfFile *)data;
    }
    else
    {
        _process->program_data.ptr = data;
    }
    _process->task = create_task(_process);
    stack_ptr = kzalloc(ZHIOS_PROGRAM_STACK_SIZE);
    if (!stack_ptr)
    {
        res = -EMEM;
        goto out;
    }
    if (arguments)
    {
        process_init_arguments(_process, arguments);
    }
    strcpy(filename, (char *)_process->filename);
    _process->stack = stack_ptr;
    _process->id = process_id;
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
    int res = load_process(filename, process, 0);
    if (res < 0)
    {
        return res;
    }
    process_switch(*process);
    return res;
}

int process_load_with_args(const char *filename, struct Process **process, char *args)
{
    int res = load_process(filename, process, args);
    if (res < 0)
    {
        return res;
    }
    process_switch(*process);
    return res;
}

static void process_free_all_allocations(struct Process *process)
{
    for (int i = 0; i < ZHIOS_PROCESS_MAX_ALLOCATIONS; i++)
    {

        process_free_allocation(process, process->allocations[i].ptr);
    }
}

void process_switch_to_any()
{
    for (int i = 0; i < ZHIOS_MAX_PROCESS; i++)
    {
        if (process_list[i])
        {
            process_switch(process_list[i]);
            return;
        }
    }
    print("No Process to schedule...\n");
    while (1)
    {
    }
    return;
}

static void process_unlink(struct Process *process)
{
    process_list[process->id] = 0x00;
    if (process == current_process)
    {
        process_switch_to_any();
    }
}

void process_free(struct Process *process)
{
    process_free_all_allocations(process);
    loader_free_data(&process->program_data, process->file_type);
    free_task(process->task);
    kfree(process->stack);
    if (process->parameters)
    {
        kfree(process->parameters);
    }
    kfree(process);
    process_unlink(process);
}

static int process_get_allocation(struct Process *process)
{
    int res = -EMEM;
    for (int i = 0; i < ZHIOS_PROCESS_MAX_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == 0)
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
    int res = map_page_range(process->task->page_directory, ptr, ptr, size, PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);
    if (res < 0)
    {
        goto out_err;
    }
    process->allocations[ind].ptr = ptr;
    process->allocations[ind].size = size;
    return ptr;
out_err:
    kfree(ptr);
    return 0;
}

static int process_get_ptr_allocation(struct Process *process, void *ptr)
{
    int res = -EMEM;
    for (int i = 0; i < ZHIOS_PROCESS_MAX_ALLOCATIONS; i++)
    {
        if (process->allocations[i].ptr == ptr)
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
    struct ProcessAllocation *allocation = &process->allocations[ind];
    map_page_range(process->task->page_directory, allocation->ptr, allocation->ptr, allocation->size, 0x00);
    process->allocations[ind].ptr = 0x00;
    process->allocations[ind].size = 0;
    kfree(ptr);
}
