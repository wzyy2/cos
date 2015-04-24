/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef _LIBC_H
#define _LIBC_H

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MILLISECOND_PER_SECOND	1000UL
#define MICROSECOND_PER_SECOND	1000000UL
#define NANOSECOND_PER_SECOND	1000000000UL

#define MILLISECOND_PER_TICK	(MILLISECOND_PER_SECOND / CONFIG_TICK_PER_SECOND)
#define MICROSECOND_PER_TICK	(MICROSECOND_PER_SECOND / CONFIG_TICK_PER_SECOND)
#define NANOSECOND_PER_TICK		(NANOSECOND_PER_SECOND  / CONFIG_TICK_PER_SECOND)

void libc_system_init(const char* tty_name);

/* some time related function */
int libc_set_time(const struct timespec *time);
int libc_get_time(struct timespec *time);
int libc_time_to_tick(const struct timespec *time);


#ifdef __cplusplus
}
#endif

#endif
