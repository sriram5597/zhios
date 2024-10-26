#ifndef DISK_H
#define DISK_H

#include "fs/file.h"

typedef unsigned short DiskType;

#define REAL_DISK 0;

struct disk
{
    int id;
    DiskType type;
    int sector_size;
    FileSystem *fs;
    void *fs_private;
};

typedef struct disk Disk;

void disk_search_and_init();
Disk *get_disk(int index);
int disk_read_block(Disk *disk, unsigned int lba, int total, void *buffer);

#endif
