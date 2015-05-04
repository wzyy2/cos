/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <cos/cos.h>


static void shell_entry(void *p)
{
    while(1) {
        //Thread::sleep(CONFIG_TICK_PER_SECOND);
        printk("111\n");
    }
}

static void shell_entry2(void *p)
{
    while(1) {
        //Thread::sleep(CONFIG_TICK_PER_SECOND);
        printk("222\n");
    }
}
static void shell_init()
{
    Thread *shell = new Thread("shell", shell_entry, NULL, 1024 * 4, Scheduler::THREAD_PRIORITY_MAX - 2, 30);
    shell->startup();

    Thread *shell2 = new Thread("shell2", shell_entry2, NULL, 1024 * 4, Scheduler::THREAD_PRIORITY_MAX - 2, 30);
    shell2->startup();
}

INIT_COMPONENT_EXPORT(shell_init);
