#include "fat16.h"
#include "status.h"
#include "memory/memory.h"
#include "string/string.h"
#include "disk/stream.h"
#include "disk/disk.h"
#include "memory/heap/kheap.h"
#include <stdint.h>
#include "config.h"
#include "terminal/terminal.h"

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVED 0x20

#define ENTRY_FREE 0x00
#define BAD_SECTOR 0xFF7
#define FAT16_ENTRY_SIZE 0x02

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

struct Fat16Entry
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
    struct Fat16Entry *directory_item;
    int total;
    int sector_pos;
    int ending_sector_pos;
};

struct Fat16Item
{
    union
    {
        struct Fat16Directory *directory;
        struct Fat16Entry *item;
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
int fat16_read(struct disk *disk, void *descriptor, int size, char *out);

FileSystem fat16_fs = {
    .name = "FAT16",
    .fopen = fat16_open,
    .fresolve = fat16_resolve,
    .fread = fat16_read};

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

void fat16_free_directory(struct Fat16Directory *directory)
{
    if (!directory)
    {
        return;
    }
    if (directory->directory_item)
    {
        kfree(directory->directory_item);
    }
    kfree(directory);
}

void fat16_free_item(struct Fat16Item *item)
{
    if (item->type == FAT16_ITEM_TYPE_DIRECTORY)
    {
        fat16_free_directory(item->directory);
    }
    else if (item->type == FAT16_ITEM_TYPE_FILE)
    {
        kfree(item->item);
    }
    kfree(item);
}

int get_total_entries_for_directory(struct disk *disk, uint32_t start_sector)
{
    struct Fat16Entry entry;
    int res = 0;
    int i = 0;
    struct DiskStream *stream = disk_stream_open(disk->id);
    disk_stream_seek(stream, start_sector * disk->sector_size);
    memset(&entry, 0, sizeof(entry));
    while (1)
    {
        if (disk_stream_read(stream, &entry, sizeof(entry)) != 0)
        {
            res = -EIO;
            goto out;
        }
        if (entry.filename[0] == 0x00)
        {
            res = i;
            break;
        }
        if (entry.filename[0] == 0xE5)
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
    int root_dir_size = root_dir_entries * sizeof(struct Fat16Entry);
    struct Fat16Entry *entries = kzalloc(root_dir_size);
    if (!entries)
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
    if (disk_stream_read(dir_stream, entries, root_dir_size) != 0)
    {
        res = -EIO;
        goto out;
    }

    fat16_private->root_directory.directory_item = entries;
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

struct Fat16Entry *fat16_clone_directory_item(struct Fat16Entry *directory_item, int size)
{
    struct Fat16Entry *copy = 0;
    if (size < sizeof(struct Fat16Entry))
    {
        return 0;
    }
    copy = kzalloc(size);
    if (!copy)
    {
        return 0;
    }
    memcpy(directory_item, copy, size);
    return copy;
}

uint32_t get_first_cluster(struct Fat16Entry *entry)
{
    return entry->first_cluster_high_byte << 16 | entry->first_cluster_low_byte;
}

static int cluster_to_sector(struct Fat16Private *fat_private, int cluster)
{
    return fat_private->root_directory.ending_sector_pos + (cluster - 2) * fat_private->fat_header.priamry_header.sectors_per_cluster;
}

int get_fat_entry(struct disk *disk, int cluster_to_use)
{
    int res = 0;
    struct Fat16Private *private = disk->fs_private;
    struct DiskStream *stream = private->fat_read_stream;
    if (!stream)
    {
        res = -EIO;
        goto out;
    }
    uint32_t fat_position = private->fat_header.priamry_header.reserved_sector_count * disk->sector_size;
    uint32_t cluster_offset = (cluster_to_use - 1) * 2;
    disk_stream_seek(stream, fat_position + cluster_offset);
    uint16_t result = 0;
    res = disk_stream_read(stream, &result, sizeof(result));
    if (res < 0)
    {
        goto out;
    }
    res = result;
out:
    return res;
}

int get_cluster_for_offset(struct disk *disk, int cluster, int offset)
{
    int res = 0;
    struct Fat16Private *private = disk->fs_private;
    int size_of_cluster_bytes = private->fat_header.priamry_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = cluster;
    int cluster_ahead = offset / size_of_cluster_bytes;
    for (int i = 0; i < cluster_ahead; i++)
    {
        int entry = get_fat_entry(disk, cluster_to_use);
        if (entry == 0xFF8 || entry == 0xFFF)
        {
            res = -EIO;
            goto out;
        }
        if (entry == BAD_SECTOR)
        {
            res = -EIO;
            goto out;
        }
        if (entry == 0xFF0 || entry == 0xFF6)
        {
            res = -EIO;
            goto out;
        }
        if (entry == 0x00)
        {
            res = -EIO;
            goto out;
        }
        cluster_to_use = entry;
    }
    res = cluster_to_use;
out:
    return res;
}

int read_entries(struct disk *disk, struct DiskStream *stream, int cluster, int offset, int size, void *out)
{
    int res = 0;
    struct Fat16Private *fat16_private = disk->fs_private;
    int size_of_cluster_in_bytes = fat16_private->fat_header.priamry_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = get_cluster_for_offset(disk, cluster, offset);
    int offset_from_cluster = offset % size_of_cluster_in_bytes;
    int start_sector = cluster_to_sector(fat16_private, cluster_to_use);
    int total_to_read = size > size_of_cluster_in_bytes ? size_of_cluster_in_bytes : size;
    int start_pos = start_sector * disk->sector_size + offset_from_cluster;
    disk_stream_seek(stream, start_pos);
    res = disk_stream_read(stream, out, total_to_read);
    if (res < 0)
    {
        goto out;
    }
    size -= total_to_read;
    if (size > 0)
    {
        res += read_entries(disk, stream, cluster, offset + total_to_read, size, out + total_to_read);
    }
out:
    return res;
}

int load_entries_from_cluster(struct disk *disk, int start_cluster, int offset, int size, void *out)
{
    struct Fat16Private *fs_private = disk->fs_private;
    struct DiskStream *stream = fs_private->cluster_read_stream;
    return read_entries(disk, stream, start_cluster, offset, size, out);
}

struct Fat16Directory *load_directory(struct disk *disk, struct Fat16Entry *directory_item)
{
    struct Fat16Private *fat16_private = disk->fs_private;
    int res = 0;
    struct Fat16Directory *directory = kzalloc(sizeof(struct Fat16Directory));
    int cluster = get_first_cluster(directory_item);
    int sector = cluster_to_sector(fat16_private, cluster);
    directory->total = get_total_entries_for_directory(disk, sector);
    int directory_size = directory->total * sizeof(struct Fat16Entry);
    directory->directory_item = kzalloc(directory_size);
    if (!directory->directory_item)
    {
        res = -EMEM;
        goto out;
    }
    res = load_entries_from_cluster(disk, cluster, 0, directory_size, directory->directory_item);
out:
    if (res < 0)
    {
        fat16_free_directory(directory);
    }
    return directory;
}

struct Fat16Item *new_fat16_item(struct disk *disk, struct Fat16Entry *directory_item)
{
    struct Fat16Item *fat_item = kzalloc(sizeof(struct Fat16Item));
    if (!fat_item)
    {
        return 0;
    }
    if (directory_item->attribute & ATTR_DIRECTORY)
    {
        fat_item->type = FAT16_ITEM_TYPE_DIRECTORY;
        fat_item->directory = load_directory(disk, directory_item);
    }
    else
    {
        fat_item->type = FAT16_ITEM_TYPE_FILE;
        fat_item->item = fat16_clone_directory_item(directory_item, sizeof(struct Fat16Entry));
    }
    return fat_item;
}

void get_fat16_filename(struct Fat16Entry *entry, char *filename)
{
    rstrip((const char *)(entry->filename), filename);
    if (entry->extension[0] != 0x00 && entry->extension[0] != 0x20)
    {
        strcat(filename, ".", filename);
        strcat(filename, (const char *)entry->extension, filename);
        rstrip(filename, filename);
    }
}

struct Fat16Item *find_item_in_directory(struct disk *disk, struct Fat16Directory *directory, struct Path *path)
{
    const char filename[ZHIOS_MAX_PATH];
    struct Fat16Item *item = 0;
    for (int i = 0; i < directory->total; i++)
    {
        memset((void *)filename, 0, ZHIOS_MAX_PATH);
        get_fat16_filename(&directory->directory_item[i], (char *)filename);
        int path_len = strlen(path->path_name);
        int filename_len = strlen(filename);
        if (filename_len != path_len)
        {
            continue;
        }
        if (istrncmp(filename, path->path_name, strlen(filename)) == 0)
        {
            print("file found...\n");
            item = new_fat16_item(disk, &directory->directory_item[i]);
            break;
        }
    }
    if (path->next && item->type != FAT16_ITEM_TYPE_DIRECTORY)
    {
        return 0;
    }
    if (path->next)
    {
        item = find_item_in_directory(disk, item->directory, path->next);
    }
    return item;
}

struct Fat16Item *get_fat16_directory_item(struct disk *disk, struct Path *path)
{
    struct Fat16Private *fat16_private = disk->fs_private;
    struct Fat16Item *item = find_item_in_directory(disk, &fat16_private->root_directory, path);
    return item;
}

void *fat16_open(struct disk *disk, struct Path *path, FileMode mode)
{
    if (mode != FILE_MODE_READ)
    {
        return 0;
    }
    struct Fat16Descriptor *descriptor = kzalloc(sizeof(struct Fat16Descriptor));
    if (!descriptor)
    {
        return 0;
    }
    descriptor->item = get_fat16_directory_item(disk, path);
    if (!descriptor->item)
    {
        return 0;
    }
    print("File Opened: ");
    char filename[ZHIOS_MAX_PATH];
    get_fat16_filename(descriptor->item->item, filename);
    print(filename);
    print("\n");
    descriptor->pos = 0;
    return descriptor;
}

int fat16_read(struct disk *disk, void *fd, int size, char *out)
{
    int res = 0;
    struct Fat16Descriptor *descriptor = fd;
    struct Fat16Entry *entry = descriptor->item->item;
    res = load_entries_from_cluster(disk, get_first_cluster(entry), descriptor->pos, size, out);
    return res;
}
