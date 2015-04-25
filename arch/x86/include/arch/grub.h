/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef __GRUB_H__
#define __GRUB_H__

#include <cos/cosDef.h>

/* the magic number for the multiboot header.  */
#define MULTIBOOT_HEADER_MAGIC		0x1BADB002

/* the flags for the multiboot header.  */
#define MULTIBOOT_HEADER_FLAGS		0x00000007

/* the magic number passed by a multiboot-compliant boot loader.  */
#define MULTIBOOT_BOOTLOADER_MAGIC	0x2BADB002

#ifndef __ASM__
/* the multiboot header.  */
typedef struct multiboot_header
{
    unsigned long magic;
    unsigned long flags;
    unsigned long checksum;
    unsigned long header_addr;
    unsigned long load_addr;
    unsigned long load_end_addr;
    unsigned long bss_end_addr;
    unsigned long entry_addr;
} multiboot_header_t;

/* the section header table for elf.  */
typedef struct elf_section_header_table
{
    unsigned long num;
    unsigned long size;
    unsigned long addr;
    unsigned long shndx;
} elf_section_header_table_t;

/* the multiboot information.  */
typedef struct multiboot_info
{
    unsigned long flags;
    unsigned long mem_lower;
    unsigned long mem_upper;
    unsigned long boot_device;
    unsigned long cmdline;
    unsigned long mods_count;
    unsigned long mods_addr;
    union
    {
        //aout_symbol_table_t aout_sym;
        elf_section_header_table_t elf_sec;
    } u;
    unsigned long mmap_length;
    unsigned long mmap_addr;
    unsigned long drives_length;
    unsigned long drives_addr;
    unsigned long config_table;
    unsigned long boot_loader_name;
    unsigned long apm_table;
    unsigned long vbe_control_info;
    unsigned long vbe_mode_info;
    unsigned long vbe_mode;
    unsigned long vbe_interface_seg;
    unsigned long vbe_interface_off;
    unsigned long vbe_interface_len;
} multiboot_info_t;

/* the module structure.  */
typedef struct module
{
    unsigned long mod_start;
    unsigned long mod_end;
    unsigned long string;
    unsigned long reserved;
} module_t;

/* the memory map. be careful that the offset 0 is base_addr_low
   but no size.  */
typedef struct memory_map
{
    unsigned long size;
    unsigned long long int base_addr;
    unsigned long long int length;
    unsigned long type;
} memory_map_t;

typedef struct VbeInfoBlock {
    char VbeSignature[4];             // == "VESA"
    uint16_t VbeVersion;                 // == 0x0300 for VBE 3.0
    uint32_t OemStringPtr;            // isa vbeFarPtr
    uint32_t Capabilities;
    uint32_t VideoModePtr;         // isa vbeFarPtr
    uint16_t TotalMemory;             // as # of 64KB blocks
} vbe_info_t;

typedef struct ModeInfoBlock {
    uint16_t attributes;
    uint8_t winA,winB;
    uint16_t granularity;
    uint16_t winsize;
    uint16_t segmentA, segmentB;
    void *realFctPtr;
    uint16_t pitch; // bytes per scanline

    uint16_t Xres, Yres;
    uint8_t Wchar, Ychar, planes, bpp, banks;
    uint8_t memory_model, bank_size, image_pages;
    uint8_t reserved0;

    uint8_t red_mask, red_position;
    uint8_t green_mask, green_position;
    uint8_t blue_mask, blue_position;
    uint8_t rsv_mask, rsv_position;
    uint8_t directcolor_attributes;

    uint32_t physbase;  // your LFB (Linear Framebuffer) address ;)
    uint32_t reserved1;
    uint16_t reserved2;
} mode_info_t;

#ifdef __cplusplus
extern "C" {
#endif

void setup_multiboot(unsigned long magic, multiboot_info_t *mbt);

extern multiboot_info_t *boot_info;

extern vbe_info_t *vbe_info;

extern mode_info_t *display_info;

#ifdef __cplusplus
}
#endif

#endif

#endif
