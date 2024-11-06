#ifndef FILE_H
#define FILE_H

#include "path.h"

typedef unsigned int FileSeekMode;
enum
{
    SEEK_BEGIN,
    SEEK_CURRENT,
    SEEK_END
};

typedef unsigned int FileMode;
enum
{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;

typedef void *(*fs_open_function)(struct disk *disk, struct Path *path, FileMode);
typedef int (*fs_resolve_function)(struct disk *disk);

typedef struct FileSystem
{
    fs_open_function fopen;
    fs_resolve_function fresolve;
    char *name;
} FileSystem;

struct FileDescriptor
{
    int id;
    FileSystem *fs;
    void *private_data;
    struct disk *disk;
};

void fs_init();
FileSystem *fs_resolve(struct disk *disk);
int fopen(const char *filename, const char *mode);

#endif