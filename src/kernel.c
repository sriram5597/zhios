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
#include "isr80h/isr80h.h"
#include "keyboard/keyboard.h"

struct Page *page = 0;

extern void kernel_registers();

void kernel_page()
{
    kernel_registers();
    paging_switch(page);
}

void kernel_main()
{
    init_terminal();
    init_gdt();
    kheap_init();
    fs_init();
    disk_search_and_init();
    init_interrupts();
    page = init_paging(PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE);
    // enable_interrupts();
    register_system_commands();
    keyboard_init();

    struct Process *process = 0;
    int res = process_load_and_switch("0:/bin/blank.elf", &process);
    if (res < 0)
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
