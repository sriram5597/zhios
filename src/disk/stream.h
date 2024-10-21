#ifndef DISK_STREAM_H
#define DISK_STREAM_H

#include "disk.h"

struct DiskStream {
    int pos;
    Disk* disk;
};

typedef struct DiskStream DiskStream;

DiskStream* disk_stream_open(int drive_number);
void disk_stream_seek(DiskStream* stream, int pos);
int disk_stream_read(DiskStream* stream, void* out, int total);
void disk_stream_close(DiskStream* stream);

#endif