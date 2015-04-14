#include "cos/irq.h"

#include <cos/cos.h>


volatile uint8_t interrupt_nest;

/**
 * This function will be invoked by BSP, when enter interrupt service routine
 *
 * @note please don't invoke this routine in application
 *
 * @see interrupt_leave
 */
void interrupt_enter(void)
{
    base_t level;

    COS_DEBUG_LOG(COS_DEBUG_IRQ, ("irq coming..., irq nest:%d\n",
                                interrupt_nest));
    level = arch_interrupt_disable();
    interrupt_nest ++;
    arch_interrupt_enable(level);
}


/**
 * This function will be invoked by BSP, when leave interrupt service routine
 *
 * @note please don't invoke this routine in application
 *
 * @see interrupt_enter
 */
void interrupt_leave(void)
{
    base_t level;

    COS_DEBUG_LOG(COS_DEBUG_IRQ, ("irq leave, irq nest:%d\n",
                                interrupt_nest));

    level = arch_interrupt_disable();
    interrupt_nest --;
    arch_interrupt_enable(level);
}


/**
 * This function will return the nest of interrupt.
 *
 * User application can invoke this function to get whether current
 * context is interrupt context.
 *
 * @return the number of nested interrupts.
 */
uint8_t interrupt_get_nest(void)
{
    return interrupt_nest;
}
