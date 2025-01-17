#include "stdlib.h"
#include <stddef.h>

extern void *system_malloc(size_t size);
extern void system_free(void *ptr);

void *malloc(size_t size)
{
    return system_malloc(size);
}

void free(void *ptr)
{
    system_free(ptr);
}
