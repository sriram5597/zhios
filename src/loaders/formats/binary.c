#include "binary.h"
#include "fs/file.h"
#include "memory/heap/kheap.h"
#include "status.h"
#include <stdint.h>

int load_binary(const char *filename, void **data, uint32_t *size)
{
    int res = 0;
    int fd = fopen(filename, "r");
    if (fd == 0)
    {
        res = -EIO;
        goto out;
    }
    struct FileStat stat;
    res = fstat(fd, &stat);
    if (res < 0)
    {
        goto out;
    }
    void *program_data = kzalloc(stat.size);
    if (!program_data)
    {
        res = -EMEM;
        goto out;
    }
    if (fread(fd, program_data, stat.size) < 0)
    {
        res = -EIO;
        kfree(program_data);
        goto out;
    }
    *data = program_data;
    *size = stat.size;
out:
    fclose(fd);
    return res;
}
