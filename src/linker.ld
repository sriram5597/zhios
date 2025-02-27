ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
SECTIONS
{
    . = 1M;

    .text : ALIGN(4096)
    {
        *(.text)
    }

    .rodata : ALIGN(4096)
    {
        *(.rodata)
    }

    .data : ALIGN(4096)
    {
        __data_load_start = LOADADDR(.data);
        __data_start = .;
        *(.data)
        __data_end = .;
    }

    .bss : ALIGN(4096)
    {
        __bss_start = .;
        *(COMMON)
        *(.bss)
        __bss_end = .;
    }

    .asm : ALIGN(4096)
    {
        *(.asm)
    }

    PROVIDE(__bss_start = ADDR(.bss));
    PROVIDE(__bss_end = ADDR(.bss) + SIZEOF(.bss));
    PROVIDE(__data_load_start = ADDR(.data));
    PROVIDE(__data_start = ADDR(.data));
    PROVIDE(__data_end = ADDR(.data) + SIZEOF(.data));
}
