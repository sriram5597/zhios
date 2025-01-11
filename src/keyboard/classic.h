#ifndef CLASSIC_H
#define CLASSIC_H

#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE
#define KEYBOARD_INPUT_PORT 0x60
#define KEY_RELEASED 0x80
#define ISR_KEYBOARD_INTERRUPT 0x21

struct Keyboard *get_classic_keyboard();

#endif
