#ifndef PATH_H
#define PATH_H

#include <stdbool.h>

struct Path
{
    const char *path_name;
    struct Path *next;
};

struct RootPath
{
    int drive_number;
    struct Path *root;
};

struct RootPath *parse_path(const char *path, const char *current_directory);
void free_path(struct RootPath *root_path);

#endif
