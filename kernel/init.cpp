/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "cos/init.h"

#include <cos/cos.h>

#ifndef CONFIG_UNIT_TEST

extern init_fn_t __cos_init_start;
extern init_fn_t __cos_init_end;

static void rti_device_end(void)
{
    return;
}
INIT_EXPORT(rti_device_end, "2.post");


void init_entry(void *p)
{
    const init_fn_t *fn_ptr;

    for (fn_ptr = &__cos_init_rti_device_end; fn_ptr < &__cos_init_end; fn_ptr ++)
    {
        (*fn_ptr)();
    }
}

static void init_app_init()
{
    Thread *init_thread = new Thread("init_thread", init_entry, NULL, 1024 * 32, 0, 30);
    init_thread->startup();
}

/**
 * Export function runtime Initialization
 */
void export_runtime_init(void)
{
    const init_fn_t *fn_ptr;

    for (fn_ptr = &__cos_init_start; fn_ptr < &__cos_init_rti_device_end; fn_ptr ++)
    {
        (*fn_ptr)();
    }

    init_app_init();
}

#endif
