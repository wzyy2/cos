/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <arch/systick.h>
#include <arch/arch.h>

#include <cos/cos.h>


extern "C" void timer_handler(int vector, void *param)
{
    tick_increase();
}

void arch_tick_init(void)
{
    /* initialize 8253 clock to interrupt 1000 times/sec */
    outb(TIMER_MODE, TIMER_SEL0|TIMER_RATEGEN|TIMER_16BIT);
    outb(IO_TIMER1, TIMER_DIV(CONFIG_TICK_PER_SECOND) % 256);
    outb(IO_TIMER1, TIMER_DIV(CONFIG_TICK_PER_SECOND) / 256);

    /* install interrupt handler */
    arch_interrupt_install(INTTIMER0, (isr_handler_t) timer_handler,  NULL);
    arch_interrupt_umask(INTTIMER0);

}
