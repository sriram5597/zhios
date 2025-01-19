#include "terminal.h"
#include "string.h"
#include <stdint.h>
#include <stddef.h>

uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;
uint16_t terminal_color = 15;

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
    terminal_col = 0;
    terminal_row++;
}

void move_to_next_col()
{
    terminal_col++;
}

void terminal_backspace()
{
    if (terminal_row == 0 && terminal_col == 0)
    {
        return;
    }
    if (terminal_col == 0)
    {
        terminal_row -= 1;
        terminal_col = VGA_WIDTH;
    }
    terminal_col--;
    terminal_putchar(terminal_col, terminal_row, ' ', 15);
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
    terminal_putchar(terminal_col, terminal_row, c, terminal_color);
    move_to_next_col();
    if (terminal_col >= VGA_WIDTH)
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
    video_mem = (uint16_t *)(0xB8000);
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}
