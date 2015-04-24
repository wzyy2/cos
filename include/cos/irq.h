/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef _IRQ_H
#define _IRQ_H

#include <cos/cosConf.h>
#include <cos/cosDef.h>


#ifdef __cplusplus
extern "C" {
#endif

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


#endif /* _IRQ_H */
