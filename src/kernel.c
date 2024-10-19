#include "kernel.h"
#include<stdint.h>
#include<stddef.h>

#include "terminal/terminal.h"
#include "interrupts/interrupts.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/path.h"

static paging_chunk* kernel_page = 0;

void kernel_main() {
    init_terminal();
    kheap_init();
    disk_search_and_init();
    init_interrupts();
    kernel_page = init_paging(PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE);
    enable_interrupts();
    print("Hello World!\n");
    char buffer[512];
    disk_read_block(get_disk(0), 150, 1, buffer);
    print(buffer);
    print("\n");

    struct RootPath* root = parse_path("0:/home/sriram/test.txt", NULL);
    if (root) {
        struct Path* path = root -> root;
        while (path) {
            print(path -> path_name);
            print("/");
            path = path -> next;
        }
        print("\n");
    }
}
