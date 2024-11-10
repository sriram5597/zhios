#include "kernel.h"
#include <stdint.h>
#include <stddef.h>

#include "terminal/terminal.h"
#include "interrupts/interrupts.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/path.h"
#include "fs/file.h"
#include "disk/stream.h"

static paging_chunk *kernel_page = 0;

void kernel_main()
{
    init_terminal();
    kheap_init();
    fs_init();
    disk_search_and_init();
    init_interrupts();
    kernel_page = init_paging(PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE);
    enable_interrupts();
    print("Hello World!\n");
    int fd = fopen("0:/root/files/test.txt", "r");
    fseek(fd, 10, SEEK_BEGIN);
    char buffer[10];
    fread(fd, buffer, 10);
    buffer[10] = 0x00;
    print("Read from disk: ");
    print(buffer);
    print("\n");
}
