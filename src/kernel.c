#include "kernel.h"
#include<stdint.h>
#include<stddef.h>

#include "terminal/terminal.h"
#include "interrupts/interrupts.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "fs/path.h"
#include "disk/stream.h"

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
    DiskStream* stream = disk_stream_open(0);
    disk_stream_seek(stream, 0x12c00);
    disk_stream_read(stream, buffer, 20);
    print(buffer);
    print("\n");
}
