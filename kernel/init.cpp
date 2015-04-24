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


/**
 * Export function runtime Initialization
 */
void export_runtime_init(void)
{

    const init_fn_t *fn_ptr;

    for (fn_ptr = &__cos_init_start; fn_ptr < &__cos_init_end; fn_ptr ++)
    {
        (*fn_ptr)();
    }

}

#endif
