/* the magic number for the multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC      0x1BADB002

/* the flags for the multiboot header.  */
#define MULTIBOOT_HEADER_FLAGS      0x00000007

/* 16KB */
#define CONFIG_STACKSIZE            0x4000

/**
 * @addtogroup x86
 */
/*@{*/

.section .init, "ax"

/* the system entry */
.globl _start
_start:
    jmp multiboot_entry

    /* Align 32 bits boundary.  */
    .align  4

    /* multiboot header.  */
multiboot_header:
    /* magic */
    .long   MULTIBOOT_HEADER_MAGIC
    /* flags */
    .long   MULTIBOOT_HEADER_FLAGS
    /* checksum */
    .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    .long 0, 0, 0, 0, 0
    /* mode_type */
    .long   0x00
    /* width */
    .long   640
    /* height */
    .long   480
    /* depth */
    .long   16

multiboot_entry:
    movl    $(_end + CONFIG_STACKSIZE),%esp

    /* reset eflags.  */
    pushl   $0
    popf

    /* push the pointer to the multiboot information structure.  */
    pushl   %ebx

    /* push the magic value.  */
    pushl   %eax

    /*rebuild globe describe table*/
    lgdt    mygdtdesc

    movl    $0x10,%eax
    movw    %ax,%ds
    movw    %ax,%es
    movw    %ax,%ss
    ljmp    $0x08, $relocated

relocated:
    call    main

    /* never get here */
spin:
    hlt
    jmp spin

.data
.p2align    2
mygdt:
    .word   0,0,0,0    /*16bytes*/

    .word   0xFFFF      /*4Gb - (0x100000*0x1000 = 4Gb)*/
    .word   0x0000      # base address = 0
    .word   0x9A00      /* code read/exec */
    .word   0x00C0

    .word   0xFFFF      # 4Gb - (0x100000*0x1000 = 4Gb)
    .word   0x0000      # base address = 0
    .word   0x9200      /* data read/write */
    .word   0x00C0

mygdtdesc:
    .word   0x17
    .long   mygdt


/*@}*/
