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