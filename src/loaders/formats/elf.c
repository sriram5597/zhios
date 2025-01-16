#include "elf.h"
#include <stdint.h>

uint32_t elf_get_entry_addr(struct ElfHeader *elf_header)
{
    return elf_header->e_entry;
}
