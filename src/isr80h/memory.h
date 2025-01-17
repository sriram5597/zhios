#ifndef ISR_MEMORY_H
#define ISR_MEMORY_H

#include <stddef.h>

struct InterruptFrame;
void *isr80h_malloc_command(struct InterruptFrame *frame);
void *isr80h_free_command(struct InterruptFrame *frame);

#endif