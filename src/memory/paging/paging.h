#ifndef PAGING_H
#define PAGING_H

#define PAGING_CACHE_DISABLED 0b000010000
#define PAGING_WRITE_THROUGH 0b000001000
#define PAGING_ACCESS_FROM_ALL 0b000000100
#define PAGING_IS_WRITABLE 0b000000010
#define PAGING_IS_PRESENT 0b000000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

#include <stdint.h>

struct Page
{
    uint32_t *directory_entry;
};

#include <stdbool.h>

struct Page *paging_new_chunk(uint8_t flags);
uint32_t *paging_get_directory(struct Page *chunk);
void enable_paging();
void paging_switch(struct Page *page);
int map_page(struct Page *page, void *virtual_address, void *phy_address, int flags);
int map_page_range(struct Page *page, void *virtual_address, void *phy_address, int count, int flags);
void *align_to_paging_address(void *ptr);
struct Page *init_paging(uint8_t flags);
void free_page(struct Page *page);

#endif
