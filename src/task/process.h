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

struct Process
{
    uint16_t id;
    char filename[ZHIOS_MAX_PATH];
    struct Task *task;
    struct ProcessAllocation allocations[ZHIOS_PROCESS_MAX_ALLOCATIONS];
    void *stack;
    union
    {
        void *ptr;
        struct ElfFile *elf_file;
    };
    uint32_t size;
    enum ProcessFileType file_type;
    struct KeyboardBuffer keyboard_buffer;
};

int load_process(const char *filename, struct Process **process);
void free_process(int process_id);
struct Process *get_process(int process_id);
struct Process *get_current_process();
int process_load_and_switch(const char *filename, struct Process **process);
void *process_malloc(struct Process *process, size_t size);
void process_free_allocation(struct Process *process, void *ptr);

#endif
