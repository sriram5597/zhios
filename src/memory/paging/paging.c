#include "paging.h"
#include <stdint.h>
#include "status.h"

#include "memory/heap/kheap.h"

void page_load_directory(uint32_t *directory);

static uint32_t *current_directory;

struct Page *paging_new_chunk(uint8_t flags)
{
    uint32_t *directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        uint32_t *entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++)
        {
            entry[j] = (offset + j * PAGING_PAGE_SIZE) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITABLE;
    }
    struct Page *page_chunk = kzalloc(sizeof(struct Page));
    page_chunk->directory_entry = directory;
    return page_chunk;
}

uint32_t *paging_get_directory(struct Page *chunk)
{
    return chunk->directory_entry;
}

void paging_switch(struct Page *page)
{
    page_load_directory(page->directory_entry);
    current_directory = page->directory_entry;
}

bool is_page_aligned(void *directory)
{
    return (uint32_t)directory % PAGING_PAGE_SIZE == 0;
}

void paging_get_index(void *virtual_address, uint32_t *directory_index, uint32_t *table_index)
{
    *directory_index = (uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
    *table_index = (uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE;
}

uint32_t get_page(uint32_t *directory, void *virtual_address)
{
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_index(virtual_address, &directory_index, &table_index);
    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    return table[table_index];
}

int set_page(uint32_t *directory, void *virtual_address, uint32_t phy_address)
{
    int res = 0;
    if (!(is_page_aligned(virtual_address)))
    {
        res = -EINARG;
        goto out;
    }
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_index(virtual_address, &directory_index, &table_index);
    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0xfffff000);
    table[table_index] = phy_address;
out:
    return res;
}

int map_page(struct Page *page, void *virtual_address, void *physical_address, int flags)
{
    if ((uint32_t)virtual_address % PAGING_PAGE_SIZE || (uint32_t)physical_address % PAGING_PAGE_SIZE)
    {
        return -EINARG;
    }
    return set_page(page->directory_entry, virtual_address, (uint32_t)physical_address | flags);
}

int map_page_range(struct Page *page, void *virtual_address, void *physical_address, int size, int flags)
{
    int count = (align_to_paging_address(physical_address + size) - physical_address) / PAGING_PAGE_SIZE;
    int res = 0;
    for (int i = 0; i < count; i++)
    {
        res = map_page(page, virtual_address, physical_address, flags);
        if (res < 0)
        {
            break;
        }
        virtual_address += PAGING_PAGE_SIZE;
        physical_address += PAGING_PAGE_SIZE;
    }
    return res;
}

struct Page *init_paging(uint8_t flags)
{
    struct Page *new_page = paging_new_chunk(flags);
    paging_switch(new_page);
    enable_paging();
    return new_page;
}

void free_page(struct Page *page)
{
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        uint32_t entry = page->directory_entry[i];
        uint32_t *table = (uint32_t *)(entry & 0xfffff000);
        kfree(table);
    }
    kfree(page->directory_entry);
    kfree(page);
}

void *align_to_paging_address(void *ptr)
{
    uint32_t address = (uint32_t)ptr + (PAGING_PAGE_SIZE - (uint32_t)ptr % PAGING_PAGE_SIZE);
    return (void *)address;
}

void *algin_to_lower_page(void *ptr)
{
    uint32_t address = (uint32_t)ptr - (uint32_t)ptr % PAGING_PAGE_SIZE;
    return (void *)address;
}

uint32_t paging_get_physical_address(struct Page *page, void *virtual)
{
    uint32_t offset = (uint32_t) virtual % PAGING_PAGE_SIZE;
    uint32_t page_start_address = (uint32_t) virtual - offset;
    uint32_t base_address = get_page(page->directory_entry, (void *)page_start_address) & ~0xFFF;
    return base_address + offset;
}
