#include "path.h"
#include "string/string.h"
#include "memory/memory.h"
#include "status.h"
#include "config.h"
#include "memory/heap/kheap.h"
#include "terminal/terminal.h"

bool path_is_valid_format(const char *path)
{
    int len = strlen(path);
    return len <= ZHIOS_MAX_PATH && len >= 3 && is_digit(path[0]) && memcmp((void *)&path[1], ":/", 2) == 0;
}

int path_get_drive_number(const char *path)
{
    int drive_number = atoi(path[0]);
    return drive_number;
}

struct RootPath *create_root_path(const char *path)
{
    struct RootPath *root_path = kzalloc(sizeof(struct RootPath));
    root_path->drive_number = path_get_drive_number(path);
    root_path->root = 0;
    return root_path;
}

int get_path_depth(const char *path)
{
    int depth = 1;
    int i = 0;
    while (path[i] != 0x00)
    {
        if (i > 0 && path[i] == '/')
        {
            depth++;
        }
        i++;
    }
    return depth;
}

const char *get_subpath(const char *path, int start, int end)
{
    char *subpath = kzalloc(ZHIOS_MAX_PATH);
    memcpy((void *)&path[start], subpath, (end - start) + 1);
    return subpath;
}

static struct Path *create_path(const char *path)
{
    struct Path *head = NULL;
    struct Path *current = NULL;
    int depth = get_path_depth(path);
    struct Path *paths = kzalloc(sizeof(struct Path) * depth);
    int count = 0;
    int start = -1;
    int end = -1;
    for (int i = 0; i <= strlen(path); i++)
    {
        if (path[i] != '/' && path[i] != 0x00)
        {
            if (start == -1)
            {
                start = i;
            }
            end = i;
        }
        else if (start != -1)
        {
            paths[count].path_name = get_subpath(path, start, end);
            if (current != NULL)
            {
                current->next = &paths[count];
            }
            start = -1;
            current = &paths[count];
            if (head == NULL)
            {
                head = current;
            }
            count++;
        }
    }
    return head;
}

struct RootPath *parse_path(const char *path, const char *current_directory)
{
    struct RootPath *root_path = 0;
    if (!path_is_valid_format(path))
    {
        goto out;
    }
    root_path = create_root_path(path);
    struct Path *first_path = create_path(&path[2]);
    root_path->root = first_path;
out:
    return root_path;
}

void free_path(struct RootPath *root_path)
{
    struct Path *path = root_path->root;
    while (path)
    {
        struct Path *next_path = path->next;
        kfree(path);
        kfree((void *)path->path_name);
        kfree(path);
        path = next_path;
    }
    kfree(root_path);
}
