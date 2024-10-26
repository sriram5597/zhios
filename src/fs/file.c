#include "file.h"
#include "status.h"
#include "config.h"
#include "fs/fat/fat16.h"
#include "memory/memory.h"
#include "memory/heap/kheap.h"

FileSystem *file_systems[ZHIOS_MAX_FILE_SYSTEMS];
FileDescriptor *file_descriptors[ZHIOS_MAX_FILE_DESCRIPTORS];

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

FileDescriptor *get_file_descriptor(int id)
{
    if (id > ZHIOS_MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }
    int index = id - 1;
    return file_descriptors[index];
}

int new_file_descriptor(FileDescriptor *fd)
{
    int res = -EMEM;
    for (int i = 0; i < ZHIOS_MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0)
        {
            FileDescriptor *desc = kzalloc(sizeof(FileDescriptor));
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

int fopen(const char* filename, FileMode mode)
{
    return -EIO;
}
