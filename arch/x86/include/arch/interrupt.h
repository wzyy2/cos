/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include <cos/cosConf.h>
#include <cos/cosDef.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Interrupt handler definition
 */
typedef void (*isr_handler_t)(int vector, void *param);

/*
 * Interrupt interfaces
 */
void arch_interrupt_init();
void arch_interrupt_mask(int vector);
void arch_interrupt_umask(int vector);

isr_handler_t arch_interrupt_install(int              vector,
                            isr_handler_t handler,
                            void            *param);

void arch_interrupt_enable(base_t);
base_t arch_interrupt_disable();




#ifdef __cplusplus
}
#endif


#endif /* _INTERRUPT_H */
