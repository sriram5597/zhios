#include "stream.h"
#include "memory/heap/kheap.h"
#include "config.h"

DiskStream *disk_stream_open(int drive_number)
{
    Disk *disk = get_disk(drive_number);
    if (disk == 0)
    {
        return 0;
    }
    DiskStream *stream = kzalloc(sizeof(DiskStream));
    stream->pos = 0;
    stream->disk = disk;
    return stream;
}

int disk_stream_seek(DiskStream *stream, int pos)
{
    stream->pos = pos;
    return 0;
}

int disk_stream_read(DiskStream *stream, void *out, int total)
{
    int sector = stream->pos / ZHIOS_DISK_SECTOR_SIZE;
    int offset = stream->pos % ZHIOS_DISK_SECTOR_SIZE;
    char buffer[ZHIOS_DISK_SECTOR_SIZE];
    if (total <= 0)
    {
        return 0;
    }

    int res = disk_read_block(stream->disk, sector, 1, buffer);
    if (res < 0)
    {
        goto out;
    }
    int total_to_read = total > ZHIOS_DISK_SECTOR_SIZE ? ZHIOS_DISK_SECTOR_SIZE : total;
    for (int i = 0; i < total_to_read; i++)
    {
        *(char *)out = buffer[offset + i];
        out++;
    }
    stream->pos += total_to_read;
    if (total > ZHIOS_DISK_SECTOR_SIZE)
    {
        res = disk_stream_read(stream, out, total - ZHIOS_DISK_SECTOR_SIZE);
    }
out:
    return res;
}

void disk_stream_close(DiskStream *stream)
{
    kfree(stream);
}
