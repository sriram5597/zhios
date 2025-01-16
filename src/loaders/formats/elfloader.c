#include "elfloader.h"
#include "elf.h"
#include "status.h"
#include <stdbool.h>
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "fs/file.h"

char ELF_SIGNATURE[] = {0x7f, 'E', 'L', 'F'};

static bool elf_is_valid_signature(void *buffer)
{
    if (sizeof(ELF_SIGNATURE) != sizeof(buffer))
    {
        return false;
    }
    return memcmp((void *)ELF_SIGNATURE, buffer, sizeof(ELF_SIGNATURE)) == 0;
}

static bool elf_is_valid_class(struct ElfHeader *header)
{
    return header->e_ident[EI_CLASS] == EI_ELFCLASSNONE || header->e_ident[EI_CLASS] == EI_ELFCLASS32;
}

static bool elf_is_valid_encoding(struct ElfHeader *header)
{
    return header->e_ident[EI_DATA] == EI_ELFDATANONE || header->e_ident[EI_DATA] == EI_ELFDATA2LSB;
}

// static bool elf_is_executable(struct ElfHeader *header)
// {
//     return header->e_type == ET_EXEC && header->e_entry >= ZHIOS_PROGRAM_VIRTUAL_ADDRESS;
// }

static bool elf_has_program_header(struct ElfHeader *header)
{
    return header->e_phoff != 0;
}

void *elf_memory(struct ElfFile *file)
{
    return file->elf_memory;
}

struct ElfHeader *elf_get_header(struct ElfFile *file)
{
    return file->elf_memory;
}

struct ElfProgramHeader *elf_get_pheader_start(struct ElfHeader *header)
{
    if (header->e_phoff == 0)
    {
        return 0;
    }
    return (struct ElfProgramHeader *)((int)header + header->e_phoff);
}

struct ElfSectionHeader *elf_get_sheader_start(struct ElfHeader *header)
{
    return (struct ElfSectionHeader *)((int)header + header->e_shoff);
}

struct ElfProgramHeader *elf_get_pheader(struct ElfHeader *header, int index)
{
    return &elf_get_pheader_start(header)[index];
}

struct ElfSectionHeader *elf_get_sheader(struct ElfHeader *header, int index)
{
    return &elf_get_sheader_start(header)[index];
}

char *elf_str_table(struct ElfHeader *header)
{
    struct ElfSectionHeader *section = elf_get_sheader(header, header->e_shstrndx);
    return (char *)header + section->sh_offset;
}

void *elf_get_vrirtual_base(struct ElfFile *file)
{
    return file->virtual_base_address;
}

void *elf_get_virtual_end(struct ElfFile *file)
{
    return file->virtual_end_address;
}

void *elf_get_physical_base(struct ElfFile *file)
{
    return file->physical_start_address;
}

void *elf_get_physical_end(struct ElfFile *file)
{
    return file->physical_end_address;
}

int elf_validate_load(struct ElfHeader *header)
{
    return (elf_is_valid_signature(header) && elf_is_valid_class(header) && elf_is_valid_encoding(header) && elf_has_program_header(header)) ? 0 : -EINFORMAT;
}

int elf_process_pt_load(struct ElfFile *elf_file, struct ElfProgramHeader *pheader)
{
    if (elf_file->virtual_base_address >= (void *)pheader->p_vaddr || elf_file->virtual_base_address == 0x00)
    {
        elf_file->virtual_base_address = (void *)pheader->p_vaddr;
        elf_file->physical_start_address = elf_memory(elf_file) + pheader->p_offset;
    }
    unsigned int end_virtual_address = pheader->p_vaddr + pheader->p_filesz;
    if (elf_file->virtual_end_address <= (void *)end_virtual_address || elf_file->virtual_end_address == 0x00)
    {
        elf_file->virtual_end_address = (void *)end_virtual_address;
        elf_file->physical_end_address = elf_memory(elf_file) + pheader->p_offset + pheader->p_filesz;
    }
    return 0;
}

int elf_process_pheader(struct ElfFile *elf_file, struct ElfProgramHeader *pheader)
{
    int res = 0;
    switch (pheader->p_type)
    {
    case PT_LOAD:
        res = elf_process_pt_load(elf_file, pheader);
        break;

    default:
        break;
    }
    return res;
}

int elf_process_pheaders(struct ElfFile *elf_file)
{
    int res = 0;
    struct ElfHeader *header = elf_get_header(elf_file);
    for (int i = 0; i < header->e_phnum; i++)
    {
        struct ElfProgramHeader *pheader = elf_get_pheader(header, i);
        res = elf_process_pheader(elf_file, pheader);
        if (res < 0)
        {
            break;
        }
    }
    return res;
}

int elf_process_loaded(struct ElfFile *elf_file)
{
    int res = 0;
    struct ElfHeader *header = elf_get_header(elf_file);
    res = elf_validate_load(header);
    if (res < 0)
    {
        goto out;
    }
    res = elf_process_pheaders(elf_file);
out:
    return res;
}

void *elf_get_pheader_phy_address(struct ElfFile *file, struct ElfProgramHeader *header)
{
    return elf_memory(file) + header->p_offset;
}

int elf_load(const char *filename, struct ElfFile **out)
{
    struct ElfFile *elf_file = kzalloc(sizeof(struct ElfFile));
    int res = 0;
    int fd = fopen(filename, "r");
    if (fd <= 0)
    {
        res = -EIO;
        goto out;
    }
    struct FileStat stat;
    res = fstat(fd, &stat);
    if (res < 0)
    {
        res = -EIO;
        goto out;
    }
    elf_file->elf_memory = kzalloc(stat.size);
    res = fread(fd, elf_file->elf_memory, stat.size);
    if (res < 0)
    {
        goto out;
    }
    res = elf_process_loaded(elf_file);
    if (res < 0)
    {
        goto out;
    }
    *out = elf_file;
out:
    if (elf_file && res < 0)
    {
        if (elf_file->elf_memory)
        {
            kfree(elf_file->elf_memory);
        }
        kfree(elf_file);
    }
    if (fd)
    {
        fclose(fd);
    }
    return res;
}

void elf_close(struct ElfFile *file)
{
    if (!file)
    {
        return;
    }
    kfree(file->elf_memory);
    kfree(file);
}