#ifndef TSS_H
#define TSS_H

#include <stdint.h>
#include "memory/paging/paging.h"

struct TSS
{
    uint32_t link;
    uint32_t esp0;
    uint32_t ss0;
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldtr;
    uint32_t iopb;
} __attribute__((packed));

struct Registers
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip;
    uint32_t cs;
    uint32_t esp;
    uint32_t flags;
    uint32_t ss;
};

struct Task
{
    struct paging_4gb_chunk *page_directory;
    struct Registers registers;
    struct Task *next;
    struct Task *prev;
};

void load_tss(int tss_segment);
struct Task *new_task();
struct Task *get_current_task();
struct Task *get_next_task();
int free_task(struct Task *task);

#endif
