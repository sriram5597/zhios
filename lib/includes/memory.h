#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void *memset(void *ptr, int n, size_t size);
int memcmp(void *ptr1, void *ptr2, int count);
void memcpy(void *ptr1, void *ptr2, int count);

#endif