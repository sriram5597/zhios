#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>
#include "memory/paging/paging.h"

enum ProcessFileType
{
    PROCESS_FILETYPE_ELF = 1,
    PROCESS_FILETYPE_BINARY
};

union ProgramData
{
    void *ptr;
    struct ElfFile *elf_file;
};

int load_data(const char *filename, void **data, uint32_t *size);
int loader_map_memory(struct Page *page, void *data, uint32_t size, enum ProcessFileType file_type);
void loader_free_data(union ProgramData *program_data, enum ProcessFileType file_type);

#endif