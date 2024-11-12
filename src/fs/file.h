#ifndef FILE_H
#define FILE_H

#include <stdint.h>
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

typedef unsigned int FileAttr;

enum
{
    READ_ONLY = 0b00000001
};

struct FileStat
{
    char filename[100];
    FileAttr attr;
    uint32_t size;
};

struct disk;

typedef void *(*fs_open_function)(struct disk *disk, struct Path *path, FileMode);
typedef int (*fs_resolve_function)(struct disk *disk);
typedef int (*fs_read_function)(struct disk *disk, void *private, int size, char *out);
typedef int (*fs_seek_function)(void *private, int offset);
typedef int (*fs_stat_function)(void *private, struct FileStat *stat);
typedef void (*fs_close_function)(void *private);

typedef struct FileSystem
{
    fs_open_function fopen;
    fs_resolve_function fresolve;
    fs_read_function fread;
    fs_seek_function fseek;
    fs_stat_function fstat;
    fs_close_function fclose;
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
int fread(int fd, void *buffer, int size);
int fseek(int fd, int offset, FileSeekMode mode);
int fstat(int fd, struct FileStat *stat);
void fclose(int fd);

#endif