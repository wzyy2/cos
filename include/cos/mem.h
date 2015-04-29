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

/* memory management option */
#define CONFIG_MM_PAGE_SIZE                 4096
#define CONFIG_MM_PAGE_MASK                 (CONFIG_MM_PAGE_SIZE - 1)
#define CONFIG_MM_PAGE_BITS                 12


/*
 * heap memory interface
 */
void system_heap_init(void *begin_addr, void *end_addr);

void *kmalloc(size_t nbytes);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t nbytes);
void *kcalloc(size_t count, size_t size);

void list_mem();

void *get_zeroed_pages(size_t npages);
void *page_alloc(size_t npages);
void page_free(void *addr, size_t npages);


#ifdef __cplusplus
}
#endif


#endif /* _MEMHEAP_H */
