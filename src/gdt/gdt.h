#ifndef GDT_H
#define GDT_H

#include <stdint.h>

struct GdtEntry
{
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t flags;
    uint8_t base_high;
} __attribute__((packed));

struct GdtEntity
{
    uint32_t limit;
    uint32_t base;
    uint8_t type;
} __attribute__((packed));

void init_gdt();

#endif