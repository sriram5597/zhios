#include "proc.h"

extern void system_start_process(const char *filename);
extern void system_exit_process();

int start_process(const char *filename)
{
    system_start_process(filename);
    return 0;
}

void exit_process()
{
    system_exit_process();
}