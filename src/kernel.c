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
#include "task/process.h"
#include "task/task.h"
#include "gdt/gdt.h"

static struct Page *kernel_page = 0;

void kernel_main()
{
    init_terminal();
    init_gdt();
    kheap_init();
    fs_init();
    disk_search_and_init();
    init_interrupts();
    kernel_page = init_paging(PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE);
    // enable_interrupts();

    struct Process *process = 0;
    int res = load_process("0:/bin/blank.bin", &process);
    if (res != 0)
    {
        print("Failed to load process..");
    }
    else
    {
        run_first_task();
        print("Hello World!\n");
    }
    while (1)
    {
    }
}
