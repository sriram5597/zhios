#ifndef HEAP_H
#define HEAP_H

#include "config.h"
#include<stdint.h>
#include<stddef.h>

#define HEAP_BLOCK_TAKEN 0x01
#define HEAP_BLOCK_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST 0b01000000

typedef unsigned char HEAP_BLOCK;

struct heap_table {
    size_t total;
    HEAP_BLOCK* entries;
};

struct heap {
    struct heap_table* table;
    void* start_address;
};

int heap_create(struct heap* heap, void* ptr, void* end, struct heap_table *table);
void* heap_malloc(struct heap* heap, size_t size);
void heap_free(struct heap* heap, void* ptr);

#endif
