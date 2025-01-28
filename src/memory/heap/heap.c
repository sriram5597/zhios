#include "heap.h"
#include "config.h"
#include "status.h"
#include "memory.h"
#include "terminal/terminal.h"
#include <stdbool.h>

static bool is_valid_alignment(void *ptr)
{
    return (unsigned int)ptr % ZHIOS_HEAP_BLOCK_SIZE == 0;
}

static bool is_valid_table(void *ptr, void *end, struct heap_table *table)
{
    size_t size = (size_t)(end - ptr);
    size_t total_blocks = size / ZHIOS_HEAP_BLOCK_SIZE;
    if (table->total != total_blocks)
    {
        return false;
    }
    return true;
}

int heap_create(struct heap *heap, void *ptr, void *end, struct heap_table *table)
{
    int res = 0;

    if (!is_valid_alignment(ptr) || !is_valid_alignment(end))
    {
        res = -EINARG;
        goto out;
    }
    size_t table_size = sizeof(HEAP_BLOCK) * table->total;
    memset(table->entries, HEAP_BLOCK_FREE, table_size);
    if (!is_valid_table(ptr, end, table))
    {
        res = -EINARG;
        goto out;
    }
    heap->start_address = ptr;
    heap->table = table;
out:
    return res;
}

int heap_get_entry_type(HEAP_BLOCK entry)
{
    return entry & HEAP_BLOCK_TAKEN;
}

int heap_get_free_block(struct heap *heap, uint32_t total_blocks)
{
    struct heap_table *table = heap->table;
    int block_count = 0;
    int start_block = -1;

    for (int i = 0; i < table->total; i++)
    {
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_FREE)
        {
            block_count = 0;
            start_block = -1;
            continue;
        }
        if (start_block == -1)
        {
            start_block = i;
        }
        block_count++;
        if (block_count == total_blocks)
        {
            break;
        }
    }
    if (start_block == -1)
    {
        return -EMEM;
    }
    return start_block;
}

uint32_t get_offset(int block)
{
    return ZHIOS_HEAP_BLOCK_SIZE * block;
}

void heap_allocate_blocks(struct heap *heap, int start_block, int total_blocks)
{
    int end_block = (total_blocks + start_block) - 1;
    HEAP_BLOCK entry = HEAP_BLOCK_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (total_blocks > 1)
    {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }
    for (int i = start_block; i <= end_block; i++)
    {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TAKEN;
        if (i != end_block - 1)
        {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

void *heap_malloc_blocks(struct heap *heap, uint32_t total_blocks)
{
    void *address = 0;
    int start_block = heap_get_free_block(heap, total_blocks);
    if (start_block < 0)
    {
        goto out;
    }
    address = heap->start_address + get_offset(start_block);
    heap_allocate_blocks(heap, start_block, total_blocks);
out:
    return address;
}

size_t heap_align_size(size_t size)
{
    int misaligned_size = size % ZHIOS_HEAP_BLOCK_SIZE;
    if (misaligned_size == 0)
    {
        return size;
    }
    size = size - misaligned_size;
    return size + ZHIOS_HEAP_BLOCK_SIZE;
}

int address_to_heap_block(struct heap *heap, void *address)
{
    return (int)(address - heap->start_address) / ZHIOS_HEAP_BLOCK_SIZE;
}

void heap_free_blocks(struct heap *heap, int block)
{
    for (int i = block; i < (int)heap->table->total; i++)
    {
        HEAP_BLOCK entry = heap->table->entries[i];
        heap->table->entries[i] = HEAP_BLOCK_FREE;
        if (!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }
}

void *heap_malloc(struct heap *heap, size_t size)
{
    size_t aligned_size = heap_align_size(size);
    uint32_t total_blocks = aligned_size / ZHIOS_HEAP_BLOCK_SIZE;
    return heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap *heap, void *ptr)
{
    address_to_heap_block(heap, ptr);
    heap_free_blocks(heap, address_to_heap_block(heap, ptr));
}
