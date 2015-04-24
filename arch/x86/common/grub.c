/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <arch/arch.h>
#include <cos/cos.h>

multiboot_info_t *boot_info;
vbe_info_t *vbe_info;
mode_info_t *display_info;

/* Clear the screen. */
void cls(unsigned long  data)
{
    unsigned char *videomem = (unsigned char*)data;
    unsigned long i;
    for(i=0;i<(680 * 480 * 2);i++){
        videomem[i] = 0xbb;
    }
}

void setup_multiboot(unsigned long magic, multiboot_info_t *mbt)
{
    boot_info = mbt;

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printk ("Invalid magic number: 0x%x\n", (unsigned) magic);
    }

    /* get memory map info */
    memory_map_t *mmap = (memory_map_t *)mbt->mmap_addr;
    while((unsigned long) mmap < mbt->mmap_addr + mbt->mmap_length) {
        mmap = (memory_map_t*) ( (unsigned long)mmap + mmap->size + sizeof(unsigned long) );

    }

    vbe_info = (vbe_info_t *)mbt->vbe_control_info;
    display_info = (mode_info_t *)mbt->vbe_mode_info;
}
