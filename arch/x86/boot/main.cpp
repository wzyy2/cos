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

void app_init();

void entry(void *p)
{


    while(0) {
        printk("hello! %d\n", 111);
        Thread::sleep(2000);
        //Thread::sleep(2000);
    }
}

void entry2(void *p)
{
    char buf[500];
    bzero(buf, 500);
    Device *serial = Serial::find("COM1");
    while(1) {
        //uint8_t ret=0;
//        uint8_t ret = serial->read(0, buf, 255);
//        if(ret > 0)
//            printk("hello! %d\n", buf[0]);
        Thread::sleep(200);

    }
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
    /* RAM 32M */
    system_heap_init((void *)&__bss_end, (void *)(1024UL*1024*8));

    /* init the c\c++ runtime environment */
    Runtime::boot_strap();

    /* init the console*/
    new Serial("COM1", COM1);
    console_set_device("COM1");

    /* show system version info*/
    show_version();

    /* init systick */
    arch_tick_init();

    /* export function runtime Initialization*/
    export_runtime_init();

    /* init application */
    app_init();

    /* init idle thread */
    Idle::init();

    /* start scheduler */
    Scheduler::start();

    /* never reach here */

    /* exit the c\c++ runtime environment */
    Runtime::exit();

    return 0;
}

ALIGN(4)
uint8_t thread_stack[1024 * 2];
ALIGN(4)
uint8_t thread2_stack[1024 * 2];

void app_init()
{
    Thread *test = new Thread("test", entry, NULL, &thread_stack[0] \
            , sizeof(thread_stack), Scheduler::THREAD_PRIORITY_MAX - 2, 30);
    test->startup();

    Thread *test2 = new Thread("te2st2", entry2, NULL, &thread2_stack[0] \
            , sizeof(thread2_stack), Scheduler::THREAD_PRIORITY_MAX - 3, 30);
    test2->startup();
}
