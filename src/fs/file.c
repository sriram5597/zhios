#include "file.h"
#include "status.h"
#include "config.h"
#include "fs/fat/fat16.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "fs/path.h"
#include "string/string.h"
#include "disk/disk.h"

FileSystem *file_systems[ZHIOS_MAX_FILE_SYSTEMS];
struct FileDescriptor *file_descriptors[ZHIOS_MAX_FILE_DESCRIPTORS];

FileSystem **get_free_filesystem()
{
    for (int i = 0; i < ZHIOS_MAX_FILE_SYSTEMS; i++)
    {
        if (file_systems[i] == 0)
        {
            return &file_systems[i];
        }
    }
    return 0;
}

int insert_file_system(FileSystem *new_fs)
{
    FileSystem **fs = get_free_filesystem();
    if (fs == 0)
    {
        return -EMEM;
    }
    *fs = new_fs;
    return 0;
}

static void fs_load_static()
{
    // load static file systems
    insert_file_system(fat16_init());
}

void fs_init()
{
    memset(file_systems, 0, sizeof(file_systems));
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load_static();
}

struct FileDescriptor *get_file_descriptor(int id)
{
    if (id > ZHIOS_MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }
    int index = id;
    return file_descriptors[index];
}

int new_file_descriptor(struct FileDescriptor *fd)
{
    int res = -EMEM;
    for (int i = 0; i < ZHIOS_MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0)
        {
            struct FileDescriptor *desc = kzalloc(sizeof(struct FileDescriptor));
            desc->id = i;
            file_descriptors[i] = desc;
            fd = desc;
            res = 0;
            break;
        }
    }
    return res;
}

FileSystem *fs_resolve(struct disk *disk)
{
    FileSystem *fs = 0;
    for (int i = 0; i < ZHIOS_MAX_FILE_SYSTEMS; i++)
    {
        if (file_systems[i] != 0 && file_systems[i]->fresolve(disk) == 0)
        {
            fs = file_systems[i];
            break;
        }
    }
    return fs;
}

FileMode get_file_mode_from_string(const char *mode_str)
{
    FileMode mode = FILE_MODE_INVALID;
    if (strncmp(mode_str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }
    else if (strncmp(mode_str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if (strncmp(mode_str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }
    return mode;
}

int fopen(const char *filename, const char *mode)
{
    int res = 0;
    struct RootPath *path = parse_path(filename, NULL);
    if (!path)
    {
        res = -EINARG;
        goto out;
    }
    if (!path->root)
    {
        res = -EINARG;
        goto out;
    }
    FileMode file_mode = get_file_mode_from_string(mode);
    if (file_mode == FILE_MODE_INVALID)
    {
        res = -EINARG;
        return res;
    }
    struct disk *disk = get_disk(path->drive_number);
    if (!disk)
    {
        res = -EIO;
        goto out;
    }
    if (!disk->fs)
    {
        res = -EIO;
        goto out;
    }
    void *private_descriptor_data = disk->fs->fopen(disk, path->root, file_mode);
    if (ISERR(private_descriptor_data))
    {
        res = ERROR_I(private_descriptor_data);
        goto out;
    }
    struct FileDescriptor *fd = 0;
    new_file_descriptor(fd);
    if (res < 0)
    {
        goto out;
    }
    fd->private_data = private_descriptor_data;
    fd->fs = disk->fs;
    fd->disk = disk;
    res = fd->id;
out:
    return res;
}

int fread(int fd, void *buffer, int size)
{
    int res = 0;
    if (size == 0 || fd < 0)
    {
        res = -EINARG;
        goto out;
    }
    struct FileDescriptor *descriptor = get_file_descriptor(fd);
    res = descriptor->fs->fread(descriptor->disk, descriptor->private_data, size, (char *)buffer);
out:
    return res;
}

int fseek(int fd, int offset, FileSeekMode mode)
{
    int pos = 0;
    switch (mode)
    {
    case SEEK_BEGIN:
        pos = offset;
        break;
    case SEEK_CURRENT:
        pos += offset;
        break;
    default:
        return -EINARG;
    }
    struct FileDescriptor *descriptor = get_file_descriptor(fd);
    return descriptor->fs->fseek(descriptor->private_data, pos);
}