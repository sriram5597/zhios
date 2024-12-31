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

struct paging_4gb_chunk
{
    uint32_t *directory_entry;
};

typedef struct paging_4gb_chunk paging_chunk;

#include <stdbool.h>

paging_chunk *paging_new_chunk(uint8_t flags);
uint32_t *paging_get_directory(paging_chunk *chunk);
void enable_paging();
void paging_switch(uint32_t *directory);
int map_page(uint32_t *directory, void *virtual_address, uint32_t phy_address);
paging_chunk *init_paging(uint8_t flags);
void free_page(struct paging_4gb_chunk *page);

#endif