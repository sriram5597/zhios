#ifndef PROCESS_H

#define PROCESS_H

#include <stdint.h>
#include <stddef.h>
#include <config.h>
#include "task.h"
#include "keyboard/keyboard.h"
#include "loaders/loader.h"

struct ProcessAllocation
{
    void *ptr;
    size_t size;
};

struct ProcessParameters
{
    uint32_t count;
    char *command_arguments[ZHIOS_MAX_PROCESS_ARGUMENTS];
    uint32_t start_address;
};

struct Process
{
    uint16_t id;
    char filename[ZHIOS_MAX_PATH];
    struct Task *task;
    struct ProcessAllocation allocations[ZHIOS_PROCESS_MAX_ALLOCATIONS];
    void *stack;
    union ProgramData program_data;
    uint32_t size;
    enum ProcessFileType file_type;
    struct KeyboardBuffer keyboard_buffer;
    struct ProcessParameters *parameters;
};

int load_process(const char *filename, struct Process **process, char *args);
void process_free(struct Process *process);
struct Process *get_process(int process_id);
struct Process *get_current_process();
int process_load_and_switch(const char *filename, struct Process **process);
void *process_malloc(struct Process *process, size_t size);
void process_free_allocation(struct Process *process, void *ptr);
int process_load_with_args(const char *filename, struct Process **process, char *args);

#endif
