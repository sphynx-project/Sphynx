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

    /* Text section - executable code */
    __textStart = .;
    .text : {
        *(.text .text.*)
    } :text
    __textEnd = .;

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    /* Read-only data section */
    __roDataStart = .;
    .rodata : {
        *(.rodata .rodata.*)
    } :rodata
    __roDataEnd = .;

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    /* Data section - writable data */
    __dataStart = .;
    .data : {
        *(.data .data.*)

        /* Ensure Limine bootloader markers are kept in the data section */
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    } :data
    __dataEnd = .;

    /* Dynamic section for dynamic linking or runtime use */
    .dynamic : {
        *(.dynamic)
    } :data :dynamic

    /* BSS section - uninitialized data */
    __bssStart = .;
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data
    __bssEnd = .;

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
        *(.interp)
    }
}
