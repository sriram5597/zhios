#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "terminal/terminal.h"
#include "memory.h"

struct heap *kernel_heap;
struct heap_table kernel_heap_table;

void kheap_init()
{
    void *end = (void *)(HEAP_ADDRESS + ZHIOS_HEAP_SIZE_BYTES);
    size_t total_table_entries = ZHIOS_HEAP_SIZE_BYTES / ZHIOS_HEAP_BLOCK_SIZE;
    kernel_heap_table.total = total_table_entries;
    kernel_heap_table.entries = (HEAP_BLOCK *)HEAP_TABLE_ADDRESS;
    memset(kernel_heap, 0, sizeof(struct heap));
    int res = heap_create(kernel_heap, (void *)HEAP_ADDRESS, end, &kernel_heap_table);
    if (res < 0)
    {
        print("Failed to create heap\n");
    }
}

void *kmalloc(size_t size)
{
    return heap_malloc(kernel_heap, size);
}

void kfree(void *ptr)
{
    heap_free(kernel_heap, ptr);
}

void *kzalloc(size_t size)
{
    void *address = kmalloc(size);
    if (!address)
    {
        return 0;
    }
    memset(address, 0x00, size);
    return address;
}

void *realloc(void *ptr, size_t size)
{
    void *new_ptr = kzalloc(size);
    int *cur = (int *)ptr;
    while (cur != 0)
    {
        *((int *)new_ptr) = *cur;
        cur++;
    }
    return new_ptr;
}
