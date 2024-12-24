#ifndef CONFIG_H
#define CONFIG_H

#define TOTAL_INTERRUPTS 512
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define ZHIOS_HEAP_SIZE_BYTES 104857600
#define ZHIOS_HEAP_BLOCK_SIZE 4096

#define ZHIOS_DISK_SECTOR_SIZE 512

#define ZHIOS_MAX_PATH 100

#define ZHIOS_MAX_FILE_SYSTEMS 12
#define ZHIOS_MAX_FILE_DESCRIPTORS 256

#define ZHIOS_TOTAL_GDT_SEGMENTS 6

#endif