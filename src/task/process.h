#ifndef PROCESS_H

#define PROCESS_H

#include <stdint.h>
#include <config.h>
#include "task.h"

struct Process
{
    uint16_t id;
    char filename[ZHIOS_MAX_PATH];
    struct Task *task;
    void *allocations[ZHIOS_PROCESS_MAX_ALLOCATIONS];
    void *stack;
    void *ptr;
    uint32_t size;
};

int load_process(const char *filename, struct Process **process);
void free_process(int process_id);
struct Process *get_process(int process_id);
struct Process *get_current_process();

#endif
