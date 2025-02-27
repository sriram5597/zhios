#ifndef PSF_H
#define PSF_H

#include <stdint.h>
#include "vesa/vesa.h"

#define PSF_FONT_MAGIC 0x864ab572

typedef struct
{
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t num_glyph;
    uint32_t bytes_per_glyph;
    uint32_t height;
    uint32_t width;
} __attribute__((packed)) psf_font_t;

void psf_init();
void psf_putc(char c, uint16_t x, uint16_t y, struct pixel_t fg);
void psf_clear_char(uint16_t x, uint16_t y, struct pixel_t color);
uint32_t psf_get_height();
uint32_t psf_get_width();

#endif