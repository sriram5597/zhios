#include <stdint.h>
#include <stddef.h>
#include "classic.h"
#include "keyboard.h"
#include "io/io.h"
#include "kernel.h"
#include "task/task.h"

int classic_keyboard_init();

static uint8_t keyboard_scan_set_1[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`',
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5',
    '6', '+', '1', '2', '3', '0', '.'};

struct Keyboard classic = {.name = {"Classic"}, .init = classic_keyboard_init};

void classic_keyboard_interrupt_handler();

int classic_keyboard_init()
{
    register_service_routine(ISR_KEYBOARD_INTERRUPT, classic_keyboard_interrupt_handler);
    outb(PS2_PORT, PS2_COMMAND_ENABLE_FIRST_PORT);
    return 0;
}

uint8_t classic_get_key_from_scancode(uint8_t scan_code)
{
    size_t scan_code_count = sizeof(keyboard_scan_set_1) / sizeof(uint8_t);
    if (scan_code > scan_code_count)
    {
        return 0;
    }
    char c = keyboard_scan_set_1[scan_code];
    return c;
}

struct Keyboard *get_classic_keyboard()
{
    return &classic;
}

void classic_keyboard_interrupt_handler()
{
    kernel_page();
    uint8_t scancode = 0;
    scancode = insb(KEYBOARD_INPUT_PORT);
    insb(KEYBOARD_INPUT_PORT);
    if (scancode & KEY_RELEASED)
    {
        return;
    }
    uint8_t c = classic_get_key_from_scancode(scancode);
    if (c != 0)
    {
        keyboard_push(c);
    }
    current_task_page();
    return;
}
