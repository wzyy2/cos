#ifndef _COS_H
#define _COS_H

#include <cosDebug.h>
#include <cosConf.h>
#include <cosDef.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>


/**
 * @addtogroup KernelService
 */

/*@{*/

/*
 * general kernel service
 */
void printk(const char *fmt, ...);

/*@{*/


/*
 * heap memory interface
 */
void system_heap_init(void *begin_addr, void *end_addr);

void *kmalloc(size_t nbytes);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t nbytes);
void *kcalloc(size_t count, size_t size);

/*
 * interrupt service
 */

/*
 * interrupt_enter and interrupt_leave only can be called by BSP
 */
void interrupt_enter(void);
void interrupt_leave(void);

/*
 * the number of nested interrupts.
 */
uint8_t interrupt_get_nest(void);


#ifdef __cplusplus
}
#endif





#endif /* _COS_H */
