OUTPUT_FORMAT("elf64-x86-64")
ENTRY(_start)

SECTIONS
{
    .text : ALIGN(4K)
    {
        *(.text)
        *(.text.*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data : ALIGN(4K)
    {
        *(.data)
        *(.data.*)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }

    .stack (NOLOAD) : ALIGN(4K)
    {
        . = . + 16K;
    }

    _end = .;
}
