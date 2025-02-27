#include "terminal.h"
#include "string.h"
#include <stdint.h>
#include <stddef.h>
#include "vesa/vesa.h"
#include "font/psf.h"

uint16_t *video_mem = 0;
uint16_t terminal_row = TERMINAL_MARGIN;
uint16_t terminal_col = TERMINAL_MARGIN;
uint16_t terminal_color = 15;

struct pixel_t terminal_bg = {
    .blue = 0x2B,
    .green = 0x0F,
    .red = 0x07};

struct pixel_t terminal_fg = {
    .blue = 0xD0,
    .green = 0xFD,
    .red = 0xFF};

uint16_t make_terminal_char(char c, char color)
{
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color)
{
    video_mem[VGA_WIDTH * y + x] = make_terminal_char(c, color);
}

void move_to_next_row()
{
    terminal_col = TERMINAL_MARGIN;
    terminal_row += psf_get_height() + 1;
}

void move_to_prev_row()
{
    terminal_row -= psf_get_height() - 1;
}

void move_to_next_col()
{
    terminal_col += psf_get_width() + 1;
}

void move_to_prev_col()
{
    terminal_col = terminal_col - psf_get_width() - 1;
}

void terminal_backspace()
{
    if (terminal_row == TERMINAL_MARGIN && terminal_col == TERMINAL_MARGIN)
    {
        return;
    }
    if (terminal_col == TERMINAL_MARGIN)
    {
        move_to_prev_row();
        terminal_col = get_current_video_mode()->width;
    }
    move_to_prev_col();
    psf_clear_char(terminal_col, terminal_row, terminal_bg);
}

void terminal_writechar(char c)
{
    if (c == '\n')
    {
        move_to_next_row();
        return;
    }
    if (c == 0x08)
    {
        terminal_backspace();
        return;
    }
    psf_putc(c, terminal_col, terminal_row, terminal_fg);
    move_to_next_col();
    if (terminal_col >= get_current_video_mode()->width)
    {
        move_to_next_row();
    }
}

void print(const char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        terminal_writechar(str[i]);
    }
}

void init_terminal()
{
    vesa_fill_bg(terminal_bg);
    psf_init();
    // psf_puts("Hello World!\nThis is a test", &terminal_col, &terminal_row, terminal_fg);
    // print("Hello World\nHello World\nHello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World Hello World");
}
