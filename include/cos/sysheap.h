#ifndef _MEMHEAP_H
#define _MEMHEAP_H

/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <cos/cosDef.h>


#ifdef __cplusplus
extern "C" {
#endif


/*
 * heap memory interface
 */
void system_heap_init(void *begin_addr, void *end_addr);

void *kmalloc(size_t nbytes);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t nbytes);
void *kcalloc(size_t count, size_t size);




#ifdef __cplusplus
}
#endif


#endif /* _MEMHEAP_H */
