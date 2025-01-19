#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>

void print(const char *message);
char getchar();
void putchar(char c);
int printf(const char *fmt, ...);
void read_line(char *buffer, int max, bool echo_key);

#endif