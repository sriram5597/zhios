#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stddef.h>

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7FFFFFFF

#define PF_X 0x01
#define PF_W 0x02
#define PF_R 0x04

#define SHN_UNDEF 0

#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7FFFFFFF
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xFFFFFFFF

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
#define ET_LOPROC 0xFF00
#define ET_HIPROC 0xFFFF

#define EM_NONE 0
#define EM_M32 1
#define EM_386 3

#define EI_NIDENT 16
#define EI_CLASS 4
#define EI_DATA 5

#define EI_ELFCLASSNONE 0
#define EI_ELFCLASS32 1
#define EI_ELFCLASS64 2

#define EI_ELFDATANONE 0
#define EI_ELFDATA2LSB 1
#define EI_ELFDATA2MSB 2

typedef uint32_t Elf32Addr;
typedef uint16_t Elf32Half;
typedef uint32_t Elf32Off;
typedef int32_t Elf32Sword;
typedef uint32_t Elf32Word;

struct ElfProgramHeader
{
    Elf32Word p_type;
    Elf32Off p_offset;
    Elf32Addr p_vaddr;
    Elf32Addr p_addr;
    Elf32Word p_filesz;
    Elf32Word p_memsz;
    Elf32Word p_flags;
    Elf32Word p_align;
} __attribute__((packed));

struct ElfSectionHeader
{
    Elf32Word sh_name;
    Elf32Word sh_type;
    Elf32Word sh_flags;
    Elf32Addr sh_addr;
    Elf32Off sh_offset;
    Elf32Word sh_size;
    Elf32Word sh_link;
    Elf32Word sh_info;
    Elf32Word sh_addralign;
    Elf32Word sh_entsize;
} __attribute__((packed));

struct ElfHeader
{
    unsigned char e_ident[EI_NIDENT];
    Elf32Half e_type;
    Elf32Half e_machine;
    Elf32Word e_version;
    Elf32Addr e_entry;
    Elf32Off e_phoff;
    Elf32Off e_shoff;
    Elf32Word e_flags;
    Elf32Half e_ehsize;
    Elf32Half e_phentsize;
    Elf32Half e_phnum;
    Elf32Half e_shentsize;
    Elf32Half e_shnum;
    Elf32Half e_shstrndx;
} __attribute__((packed));

struct Elf32Dyn
{
    Elf32Sword d_tag;
    union
    {
        Elf32Word d_val;
        Elf32Addr d_ptr;
    } d_un;
} __attribute__((packed));

struct Elf32Sym
{
    Elf32Word st_name;
    Elf32Addr st_value;
    Elf32Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32Half st_shndx;
} __attribute__((packed));

uint32_t elf_get_entry_addr(struct ElfHeader *elf_header);

#endif