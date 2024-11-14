#include "gdt.h"
#include "config.h"
#include "memory/memory.h"

struct GdtEntry gdt_table[ZHIOS_TOTAL_GDT_SEGMENTS];
struct GdtEntity gdt_entities[ZHIOS_TOTAL_GDT_SEGMENTS];

extern void load_gdt(struct GdtEntry *entry, int size);

void load_entries()
{
    memset(gdt_table, 0x00, sizeof(gdt_table));
    for (int i = 0; i < ZHIOS_TOTAL_GDT_SEGMENTS; i++)
    {
        struct GdtEntry entry;
        struct GdtEntity entity = gdt_entities[i];
        entry.base_low = entity.base & 0xFFFF;
        entry.base_middle = (entity.base >> 16) & 0xFF;
        entry.base_high = (entity.base >> 24) & 0xFF;

        entry.limit = (entity.limit & 0xFFFF);
        entry.flags = (entity.limit >> 16) & 0x0F;
        entry.flags |= 0xC0;
        entry.access = entity.type;
        gdt_table[i] = entry;
    }
    load_gdt(gdt_table, sizeof(gdt_table));
}

void init_gdt()
{
    gdt_entities[0] = (struct GdtEntity){
        .base = 0x00,
        .limit = 0x00,
        .type = 0x00,
    };
    gdt_entities[1] = (struct GdtEntity){
        .base = 0x00,
        .limit = 0xFFFFFFFF,
        .type = 0x9A};
    gdt_entities[2] = (struct GdtEntity){
        .base = 0x00,
        .limit = 0xFFFFFFFF,
        .type = 0x92};
    load_entries();
}
