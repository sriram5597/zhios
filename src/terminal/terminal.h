#ifndef TERMINAL_H
#define TERMINAL_H

#define VGA_WIDTH 80
#define VGA_HEIGHT 40

#define TERMINAL_MARGIN 5

void print(const char *);
void init_terminal();
void terminal_writechar(char c);

#endif