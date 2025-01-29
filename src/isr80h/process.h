#ifndef ISR80H_PROCESS_H
#define ISR80H_PROCESS_H

struct InterruptFrame;
void *isr80h_load_process_command(struct InterruptFrame *frame);
void *isr80h_exit_process_command(struct InterruptFrame *frame);

#endif
