/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <cos/cos.h>
#include <arch/arch.h>

#include "console.h"
#include "serial.h"

#include <gui_interface.h>


extern unsigned long _end;

extern unsigned char __bss_start[];
extern unsigned char __bss_end[];

/* clear .bss */
static void clear_bss(void)
{
    unsigned char *dst;
    dst = __bss_start;
    while (dst < __bss_end)
        *dst++ = 0;
}

int main(unsigned long magic, multiboot_info_t *mbt)
{
    /* clear .bss */
    clear_bss();

    /* get multiboot information*/
    setup_multiboot(magic, mbt);

    /* init hardware interrupt */
    arch_interrupt_init();

    /* init memory system */
    /* RAM bss-128M  for Zone Normal*/
    system_heap_init((void *)&__bss_end, (void *)(1024UL*1024*128));

    /* init the c\c++ runtime environment */
    Runtime::boot_strap();

    /* init the console*/
    new Serial("COM1", COM1);
    console_set_device("COM1");

    /* show system version info*/
    show_version();

    /* init systick */
    arch_tick_init();

    /* Export function runtime Initialization */
    export_runtime_init();

    /* init idle thread */
    Idle::init();

    /* start scheduler */
    Scheduler::start();

    /* never reach here */

    /* exit the c\c++ runtime environment */
    Runtime::exit();

    return 0;
}

