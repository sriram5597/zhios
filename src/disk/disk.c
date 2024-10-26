#include "disk.h"
#include "config.h"
#include "io/io.h"
#include "status.h"
#include "memory/memory.h"

Disk disk;

void disk_search_and_init()
{
    memset(&disk, 0, sizeof(disk));
    disk.id = 0;
    disk.type = REAL_DISK;
    disk.sector_size = ZHIOS_DISK_SECTOR_SIZE;
    disk.fs = fs_resolve(&disk);
}

Disk *get_disk(int index)
{
    if (index != 0)
    {
        return 0;
    }
    return &disk;
}

int disk_read_sector(unsigned int lba, int total, void *buffer)
{
    outb(0x01F6, lba >> 24 | 0xE0);
    outb(0x01F2, total);
    outb(0x01F3, (unsigned char)(lba & 0xFF));
    outb(0x01F4, (unsigned char)(lba >> 8));
    outb(0x01F5, (unsigned char)(lba >> 16));
    outb(0x01F7, 0x20);

    unsigned short *ptr = (unsigned short *)buffer;
    for (int sector = 0; sector < total; sector++)
    {
        char c = insb(0x01F7);
        while (!(c & 0x08))
        {
            c = insb(0x01F7);
        }
        for (int b = 0; b < 256; b++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }
    return 0;
}

int disk_read_block(Disk *selected_disk, unsigned int lba, int total, void *buffer)
{
    if (&disk != selected_disk)
    {
        return -EIO;
    }
    return disk_read_sector(lba, total, buffer);
}
