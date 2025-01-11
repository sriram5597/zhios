#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "config.h"

typedef int (*KEYBOARD_INIT_FUNCTION)();

struct Keyboard
{
    char name[20];
    KEYBOARD_INIT_FUNCTION init;
    struct Keyboard *next;
};

struct KeyboardBuffer
{
    int head;
    int tail;
    char buffer[ZHIOS_MAX_KEYBOARD_BUFFER_SIZE];
};

void keyboard_init();
int keyboard_insert(struct Keyboard *keyboard);
void keyboard_push(char c);
char keyboard_pop();
void keyboard_backspace(struct KeyboardBuffer *buffer);

#endif