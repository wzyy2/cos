OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(_start)

SECTIONS
{
    . = 0x00100000;
    
    . = ALIGN(4);
    /* code(readonly) */
    .text :
    {
        /* First put the multiboot header, as it is required to be put very early
           early in the image or the bootloader won't recognize the file format.
           Next we'll put the .text section. */
        KEEP(*(.init))
        KEEP(*(.fini))
        *(.text)

        /* section information for initial. */
        . = ALIGN(4);
        __cos_init_start = .;
        KEEP(*(SORT(.cos_init_fn*)))
        __cos_init_end = .;
        . = ALIGN(4);
    }

    /* Read-only data. */
    . = ALIGN(4);
    .rodata :
    {
        *(.rodata);
        __CTOR_LIST__ = .;
        LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
        KEEP(*(SORT(.ctors*)))
        LONG(0) __CTOR_END__ = .;
        __DTOR_LIST__ = .;
        LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
        KEEP(*(SORT(.dtors)))
        LONG(0) __DTOR_END__ = .;
    }


    /* Read-write data (initialized) */
    . = ALIGN(4);
    .data : 
    { 
        *(.data) 
    }

    
    /* Read-write data (uninitialized) and stack */
    . = ALIGN(4);
    __bss_start = .;
    .bss : { *(.bss) }
    __bss_end   = .;
    
    /* stabs debugging sections. */
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .stab.excl 0 : { *(.stab.excl) }
    .stab.exclstr 0 : { *(.stab.exclstr) }
    .stab.index 0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 : { *(.comment) }
    .debug_abbrev 0 : { *(.debug_abbrev) }
    .debug_info 0 : { *(.debug_info) }
    .debug_line 0 : { *(.debug_line) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    .debug_aranges 0 : { *(.debug_aranges) }

    /* 0x00000000 - 0x00100000 for stack 640kb low memory */
    _end = .;
}   
