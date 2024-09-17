OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

ENTRY(KernelEntry)

PHDRS
{
    headers PT_PHDR PHDRS;
    text    PT_LOAD FILEHDR PHDRS;
    rodata  PT_LOAD;
    data    PT_LOAD;
    dynamic PT_DYNAMIC;
}

SECTIONS
{
    . = 0xffffffff80000000 + SIZEOF_HEADERS;

    __textStart = .;
    .text : {
        *(.text .text.*)
    } :text
    __textEnd = .;

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    __roDataStart = .;
    .rodata : {
        *(.rodata .rodata.*)
    } :rodata
    __roDataEnd = .;

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    __dataStart = .;
    .data : {
        *(.data .data.*)

        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    } :data

    .dynamic : {
        *(.dynamic)
    } :data :dynamic

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data
    __dataEnd = .;

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
        *(.interp)
    }
}   