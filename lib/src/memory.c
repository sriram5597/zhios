#include "memory.h"

void *memset(void *ptr, int n, size_t size)
{
    char *ptr_ch = (char *)ptr;
    for (int i = 0; i < size; i++)
    {
        ptr_ch[i] = (char)n;
    }
    return ptr;
}

int memcmp(void *ptr1, void *ptr2, int count)
{
    char *c1 = (char *)ptr1;
    char *c2 = (char *)ptr2;

    while (count-- > 0)
    {
        if (*c1 != *c2)
        {
            return c1[-1] > c2[-1] ? 1 : -1;
        }
    }
    return 0;
}

void memcpy(void *ptr1, void *ptr2, int count)
{
    char *dest = (char *)ptr2;
    char *src = (char *)ptr1;
    for (int i = 0; i < count; i++)
    {
        dest[i] = src[i];
    }
}
