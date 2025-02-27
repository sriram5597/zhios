#include "psf.h"
#include <stdint.h>
#include "string.h"
#include "memory/heap/kheap.h"
#include "vesa/vesa.h"

extern char _binary___files_fonts_powerline_psf_start;
extern char _binary___files_fonts_powerline_psf_end;

uint16_t *unicode;
psf_font_t *font = 0;

void psf_init()
{
    uint16_t glyph = 0;
    font = (psf_font_t *)&_binary___files_fonts_powerline_psf_start;
    if (font->magic != PSF_FONT_MAGIC)
    {
        return;
    }
    if (font->flags == 0)
    {
        unicode = 0;
        return;
    }
    char *table_offset = (char *)(font + font->header_size + font->num_glyph * font->bytes_per_glyph);
    unicode = kzalloc(UINT16_MAX * sizeof(uint16_t));
    while (table_offset < (char *)&_binary___files_fonts_powerline_psf_end)
    {
        uint16_t uc = (uint16_t)(table_offset[0]);
        if (uc == 0xFF)
        {
            glyph++;
            table_offset++;
            continue;
        }
        else if (uc & 128)
        {
            if ((uc & 32) == 0)
            {
                uc = ((table_offset[0] & 0x1F) << 6) + (table_offset[1] & 0x3F);
                table_offset++;
            }
            else if ((uc & 16) == 0)
            {
                uc = ((((table_offset[0] & 0xF) << 6) + (table_offset[1] & 0x3F)) << 6) + (table_offset[2] & 0x3F);
                table_offset += 2;
            }
            else if ((uc & 8) == 0)
            {
                uc = ((((((table_offset[0] & 0x7) << 6) + (table_offset[1] & 0x3F)) << 6) + (table_offset[2] & 0x3F)) << 6) + (table_offset[3] & 0x3F);
                table_offset += 3;
            }
        }
        unicode[uc] = glyph;
        table_offset++;
    }
    return;
}

void psf_putc(char c, uint16_t x, uint16_t y, struct pixel_t fg)
{
    uint8_t bytes_per_line = (font->width + 7) / 8;
    uint16_t glyph_index = (uint16_t)c;
    uint8_t *glyph = (uint8_t *)font + font->header_size + glyph_index * font->height * bytes_per_line;
    uint8_t *line;
    float scale = 1;
    for (int row = 0; row < font->height; row++)
    {

        line = glyph + row * bytes_per_line;
        for (int col = 0; col < font->width; col++)
        {
            if (line[col / 8] & (128 >> (col % 8)))
            {
                write_to_screen(x + col * scale, y + row * scale, fg);
            }
        }
    }
}

void psf_clear_char(uint16_t x, uint16_t y, struct pixel_t color)
{
    for (int row = 0; row < font->height; row++)
    {
        for (int col = 0; col < font->width; col++)
        {
            write_to_screen(x + col, y + row, color);
        }
    }
}

uint32_t psf_get_height()
{
    return font->height;
}

uint32_t psf_get_width()
{
    return font->width;
}