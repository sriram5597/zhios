#include "loader.h"
#include "status.h"
#include "memory/paging/paging.h"
#include "loaders/formats/binary.h"
#include "loaders/formats/elfloader.h"
#include "loaders/formats/elf.h"
#include "memory/paging/paging.h"

int load_data(const char *filename, void **data, uint32_t *size)
{
    int res = 0;
    int file_type = 0;
    res = elf_load(filename, (struct ElfFile **)data);
    file_type = PROCESS_FILETYPE_ELF;
    if (res == -EINFORMAT)
    {
        res = load_binary(filename, data, size);
        file_type = PROCESS_FILETYPE_BINARY;
    }

    if (res < 0)
    {
        return res;
    }
    return file_type;
}

int process_map_memory_to_elf(struct Page *page, struct ElfFile *elf_file)
{
    int res = 0;
    struct ElfHeader *header = elf_get_header(elf_file);
    for (int i = 0; i < header->e_phnum; i++)
    {
        struct ElfProgramHeader *pheader = elf_get_pheader(header, i);
        void *phy_address = elf_get_pheader_phy_address(elf_file, pheader);
        int flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;
        if (pheader->p_flags & PF_W)
        {
            flags |= PAGING_IS_WRITABLE;
        }
        res = map_page_range(page, algin_to_lower_page((void *)pheader->p_vaddr), algin_to_lower_page(phy_address), pheader->p_memsz, flags);
        if (res < 0)
        {
            break;
        }
    }
    return res;
}

int process_map_memory_to_binary(struct Page *page, void *data, uint32_t size)
{
    int res = 0;
    int flags = PAGING_IS_PRESENT | PAGING_IS_WRITABLE | PAGING_ACCESS_FROM_ALL;
    res = map_page_range(page, (void *)ZHIOS_PROGRAM_VIRTUAL_ADDRESS, data, size, flags);
    return res;
}

int loader_map_memory(struct Page *page, void *data, uint32_t size, enum ProcessFileType file_type)
{
    int res = 0;
    switch (file_type)
    {
    case PROCESS_FILETYPE_ELF:
        res = process_map_memory_to_elf(page, (struct ElfFile *)data);
        break;
    case PROCESS_FILETYPE_BINARY:
        res = process_map_memory_to_binary(page, data, size);
        break;
    default:
        res = -EINFORMAT;
        break;
    }
    return res;
}