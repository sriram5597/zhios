#ifndef ISR_IO_H
#define ISR_IO_H
#include "interrupts/interrupts.h"

void *terminal_output(struct InterruptFrame *frame);
void *putchar(struct InterruptFrame *frame);
void *get_key_from_keyboard(struct InterruptFrame *frame);

#endif