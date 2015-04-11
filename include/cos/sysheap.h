#ifndef _MEMHEAP_H
#define _MEMHEAP_H

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