#ifndef PROCESS_H

#define PROCESS_H

#include <stdint.h>
#include <config.h>
#include "task.h"
#include "keyboard/keyboard.h"
#include "loaders/loader.h"

struct Process
{
    uint16_t id;
    char filename[ZHIOS_MAX_PATH];
    struct Task *task;
    void *allocations[ZHIOS_PROCESS_MAX_ALLOCATIONS];
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

#endif
