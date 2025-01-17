#include "stdlib.h"
#include <stdbool.h>
#include <stddef.h>

extern void *system_malloc(size_t size);
extern void system_free(void *ptr);

char *itoa(int num)
{
    static char text[12];
    int loc = 11;
    text[loc] = '\0';
    bool is_negative = false;
    if (num < 0)
    {
        is_negative = true;
        num = -num;
    }
    while (num)
    {
        text[--loc] = '0' + (num % 10);
        num /= 10;
    }
    if (loc == 11)
    {
        text[--loc] = '0';
    }
    if (is_negative)
    {
        text[--loc] = '-';
    }
    return &text[loc];
}

void *malloc(size_t size)
{
    return system_malloc(size);
}

void free(void *ptr)
{
    system_free(ptr);
}
