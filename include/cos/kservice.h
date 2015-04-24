/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef _KSERVICE_H
#define _KSERVICE_H

#include <cos/cosDef.h>



#ifdef __cplusplus
extern "C" {
#endif

/*
 * general kernel service
 */
void show_version(void);

void printk(const char *fmt, ...);

err_t get_errno(void);
void set_errno(err_t error);
int *cos_errno(void);

#ifdef __cplusplus
class Device;

Device *console_set_device(const char *name);
Device *console_get_device(void);
#endif

#ifdef __cplusplus
}
#endif


#endif /* _KSERVICE_H */
