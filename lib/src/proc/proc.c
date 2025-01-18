#include "proc.h"

extern void system_start_process(const char *filename);

int start_process(const char *filename)
{
    system_start_process(filename);
    return 0;
}