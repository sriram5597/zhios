#ifndef DISK_H
#define DISK_H

typedef unsigned short DiskType;

#define REAL_DISK 0;

struct disk {
    DiskType type;
    int sector_size;
};

typedef struct disk Disk;

void disk_search_and_init();
Disk* get_disk(int index);
int disk_read_block(Disk* disk, unsigned int lba, int total, void* buffer);

#endif
