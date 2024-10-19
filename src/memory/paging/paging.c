#include "paging.h"
#include<stdint.h>
#include "status.h"

#include "memory/heap/kheap.h"

void page_load_directory(uint32_t* directory);

static uint32_t* current_directory;

paging_chunk* paging_new_chunk(uint8_t flags) {
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++) {
        uint32_t* entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++) {
            entry[j] = (offset + j * PAGING_PAGE_SIZE) | flags;
        }
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (uint32_t) entry | flags | PAGING_IS_WRITABLE;
    }
    paging_chunk* page_chunk = kzalloc(sizeof(paging_chunk));
    page_chunk -> directory_entry = directory;
    return page_chunk;
}

uint32_t* paging_get_directory(paging_chunk* chunk) {
    return chunk -> directory_entry;
}

void paging_switch(uint32_t* directory) {
    page_load_directory(directory);
    current_directory = directory;
}

bool is_page_aligned(void* directory) {
    return (uint32_t) directory % PAGING_PAGE_SIZE == 0;
}

void paging_get_index(void* virtual_address, uint32_t* directory_index, uint32_t* table_index) {
    *directory_index = (uint32_t) virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
    *table_index = (uint32_t) virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE;
}

int map_page(uint32_t* directory, void* virtual_address, uint32_t phy_address) {
    int res = 0;
    if (!(is_page_aligned(virtual_address))) {
        res = -EINARG;
        goto out;
    }
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_index(virtual_address, &directory_index, &table_index);
    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xfffff000);
    table[table_index] = phy_address;
out:
    return res;
}

paging_chunk* init_paging(uint8_t flags) {
    paging_chunk* new_page = paging_new_chunk(flags);
    paging_switch(paging_get_directory(new_page));
    enable_paging();
    return new_page;
}
