#include "vesa.h"
#include <stddef.h>

struct vbe_mode_info_t *current_mode = (struct vbe_mode_info_t *)0x5000;

void write_to_screen(int x, int y, struct pixel_t pixel_t)
{
    uint8_t *fb = (uint8_t *)current_mode->framebuffer;
    uint32_t offset = y * current_mode->pitch + x * (current_mode->bpp / 8);
    fb[offset] = pixel_t.blue;
    fb[offset + 1] = pixel_t.green;
    fb[offset + 2] = pixel_t.red;
}

void vesa_fill_bg(struct pixel_t bg)
{
    for (int y = 0; y < current_mode->height; y++)
    {
        for (int x = 0; x < current_mode->pitch; x++)
        {
            write_to_screen(x, y, bg);
        }
    }
}

struct vbe_mode_info_t *get_current_video_mode()
{
    return current_mode;
}