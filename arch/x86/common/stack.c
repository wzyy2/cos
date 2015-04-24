/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <arch/context.h>
#include <cos/cos.h>


/**
 * This function will initialize thread stack
 *
 * @param tentry the entry of thread
 * @param parameter the parameter of entry
 * @param stack_addr the beginning stack address
 * @param texit the function will be called when thread exit
 *
 * @return stack address
 */
uint8_t *arch_stack_init(void *tentry, void *parameter,
    uint8_t *stack_addr, void *texit)
{
    unsigned long *stk;

    stk = (unsigned long *)stack_addr;
    *(--stk) = (unsigned long)parameter;
    *(--stk) = (unsigned long)texit;
    *(--stk) = 0x200;						/*flags*/
    *(--stk) = 0x08;						/*cs*/
    *(--stk) = (unsigned long)tentry;		/*eip*/
    *(--stk) = 0;							/*irqno*/
    *(--stk) = 0x10;						/*ds*/
    *(--stk) = 0x10;						/*es*/
    *(--stk) = 0;							/*eax*/
    *(--stk) = 0;							/*ecx*/
    *(--stk) = 0;							/*edx*/
    *(--stk) = 0;							/*ebx*/
    *(--stk) = 0;							/*esp*/
    *(--stk) = 0;							/*ebp*/
    *(--stk) = 0;							/*esi*/
    *(--stk) = 0;							/*edi*/

    /* return task's current stack address */
    return (uint8_t *)stk;
}
