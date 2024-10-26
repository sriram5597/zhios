#include "fat16.h"
#include "status.h"
#include "memory/memory.h"
#include "string/string.h"
#include "disk/stream.h"
#include "disk/disk.h"
#include "memory/heap/kheap.h"
#include <stdint.h>

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVED 0x20

#define ENTRY_FREE 0x00

#define CLN_BIT_MASK 0x80
#define ERR_DISK_MASK 0x40

typedef unsigned int Fat16ItemType;
#define FAT16_ITEM_TYPE_DIRECTORY 0
#define FAT16_ITEM_TYPE_FILE 1

#define FAT16_BOOT_SIGNATURE 0x29

struct Fat16HeaderExtended
{
    uint8_t drive_number;
    uint8_t reserved;
    uint8_t boot_signature;
    uint32_t volume_id;
    uint8_t volume_label[11];
    uint8_t fs_type[8];
} __attribute__((packed));

struct Fat16PrimaryHeader
{
    uint8_t shot_jmp_ins[3];
    uint8_t oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t number_of_fat;
    uint16_t root_entry_count;
    uint16_t total_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t sectors_long;
} __attribute__((packed));

struct Fat16Header
{
    struct Fat16PrimaryHeader priamry_header;
    union ExtendedH
    {
        struct Fat16HeaderExtended extended_header;
    } shared;
};

struct Fat16DirectoryItem
{
    uint8_t filename[8];
    uint8_t extension[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t creation_time_tenth_of_seconds;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_accessed_date;
    uint16_t first_cluster_high_byte;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t first_cluster_low_byte;
    uint32_t file_size;
} __attribute__((packed));

struct Fat16Directory
{
    struct Fat16DirectoryItem *directory_item;
    int total;
    int sector_pos;
    int ending_sector_pos;
};

struct Fat16Item
{
    union
    {
        struct Fat16Directory *directory;
        struct Fat16Item *item;
    };
    Fat16ItemType type;
};

struct Fat16Descriptor
{
    struct Fat16Item *item;
    uint32_t pos;
};

struct Fat16Private
{
    struct Fat16Header fat_header;
    struct Fat16Directory root_directory;
    struct DiskStream *cluster_read_stream;
    struct DiskStream *fat_read_stream;
    struct DiskStream *directory_stream;
};

void *fat16_open(struct disk *disk, struct Path *path, FileMode mode);
int fat16_resolve(struct disk *disk);

FileSystem fat16_fs = {
    .name = "FAT16",
    .fopen = fat16_open,
    .fresolve = fat16_resolve};

FileSystem *fat16_init()
{
    return &fat16_fs;
}

int init_fat16_private(struct Fat16Private *fat16_private, struct disk *disk)
{
    fat16_private->cluster_read_stream = disk_stream_open(disk->id);
    fat16_private->fat_read_stream = disk_stream_open(disk->id);
    fat16_private->directory_stream = disk_stream_open(disk->id);
    if (fat16_private->cluster_read_stream == 0 || fat16_private->fat_read_stream == 0 || fat16_private->directory_stream == 0)
    {
        return -EIO;
    }
    return 0;
}

int verify_signature(struct Fat16Header fat16_header)
{
    uint8_t signature = fat16_header.shared.extended_header.boot_signature;
    if (signature != FAT16_BOOT_SIGNATURE)
    {
        return -EINARG;
    }
    return 0;
}

int get_total_entries_for_directory(struct disk *disk, uint32_t start_sector)
{
    struct Fat16DirectoryItem item;
    int res = 0;
    int i = 0;
    struct DiskStream *stream = disk_stream_open(disk->id);
    disk_stream_seek(stream, start_sector * disk->sector_size);
    memset(&item, 0, sizeof(item));
    while (1)
    {
        if (disk_stream_read(stream, &item, sizeof(item)) != 0)
        {
            res = -EIO;
            goto out;
        }
        if (item.filename[0] == 0x00)
        {
            res = i;
            break;
        }
        if (item.filename[0] == 0xE5)
        {
            continue;
        }
        i++;
    }
out:
    disk_stream_close(stream);
    return res;
}

int init_root_directory(struct Fat16Private *fat16_private, struct disk *disk)
{
    int res = 0;
    struct Fat16PrimaryHeader primary_header = fat16_private->fat_header.priamry_header;
    int root_directory_sector_start = primary_header.reserved_sector_count + (primary_header.number_of_fat * primary_header.sectors_per_fat);
    int root_dir_entries = primary_header.root_entry_count;
    int root_dir_size = root_dir_entries * sizeof(struct Fat16DirectoryItem);
    struct Fat16DirectoryItem *dir = kzalloc(root_dir_size);
    if (!dir)
    {
        res = -EMEM;
        goto out;
    }
    struct DiskStream *dir_stream = fat16_private->directory_stream;
    if (disk_stream_seek(dir_stream, root_directory_sector_start * disk->sector_size) != 0)
    {
        res = -EIO;
        goto out;
    }
    if (disk_stream_read(dir_stream, dir, root_dir_size) != 0)
    {
        res = -EIO;
        goto out;
    }

    fat16_private->root_directory.directory_item = dir;
    fat16_private->root_directory.total = get_total_entries_for_directory(disk, root_directory_sector_start);
    fat16_private->root_directory.sector_pos = root_directory_sector_start;
    fat16_private->root_directory.ending_sector_pos = root_directory_sector_start + (root_dir_size / disk->sector_size);
out:
    return res;
}

int fat16_resolve(struct disk *disk)
{
    int res = 0;
    struct Fat16Private *fat16_private = kzalloc(sizeof(struct Fat16Private));
    res = init_fat16_private(fat16_private, disk);
    if (res != 0)
    {
        goto out;
    }
    disk->fs_private = fat16_private;
    disk->fs = &fat16_fs;
    struct DiskStream *stream = disk_stream_open(disk->id);
    if (stream == 0)
    {
        res = -EMEM;
        goto out;
    }
    if (disk_stream_read(stream, &fat16_private->fat_header, sizeof(fat16_private->fat_header)) != 0)
    {
        res = -EIO;
        goto out;
    }
    res = verify_signature(fat16_private->fat_header);
    if (res != 0)
    {
        goto out;
    }
    res = init_root_directory(fat16_private, disk);
    if (res != 0)
    {
        goto out;
    }
out:
    disk_stream_close(stream);
    if (res < 0)
    {
        kfree(fat16_private);
        disk->fs = 0;
        disk->fs_private = 0x00;
    }
    return res;
}

void *fat16_open(struct disk *disk, struct Path *path, FileMode mode)
{
    return 0;
}
