#ifndef ELFLOADER_H
#define ELFLOADER_H

#include "config.h"
#include "elf.h"

struct ElfFile
{
    char filename[ZHIOS_MAX_PATH];
    int in_memory_size;

    // Physical address of elf file loaded into memory
    void *elf_memory;

    // Virtual address of the elf file loaded into memory
    void *virtual_base_address;

    // Virtual address of the end of elf file
    void *virtual_end_address;

    // Physical start address
    void *physical_start_address;

    // Physical end address
    void *physical_end_address;
};

int elf_load(const char *filename, struct ElfFile **out);
void elf_close(struct ElfFile *file);
void *elf_get_vrirtual_base(struct ElfFile *file);
void *elf_get_physical_base(struct ElfFile *file);
void *elf_get_physical_end(struct ElfFile *file);
struct ElfProgramHeader *elf_get_pheader(struct ElfHeader *header, int index);
struct ElfHeader *elf_get_header(struct ElfFile *file);
void *elf_get_pheader_phy_address(struct ElfFile *file, struct ElfProgramHeader *header);

#endif