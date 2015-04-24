/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef SYSTICK
#define SYSTICK
#include <cos/cosConf.h>
#include <cos/cosDef.h>


#ifdef __cplusplus
extern "C" {
#endif


void arch_tick_init(void);

#ifdef __cplusplus
}
#endif

#endif // SYSTICK

