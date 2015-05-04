/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <cos/clock.h>
#include <cos/cos.h>

/**
 * @addtogroup Clock
 */

/*@{*/

static tick_t global_tick = 0;

/**
 * This function will return current tick from operating system startup
 *
 * @return current tick
 */
tick_t tick_get(void)
{
    /* return the global tick */
    return global_tick;
}


/**
 * This function will set current tick
 */
void tick_set(tick_t tick)
{
    base_t level;

    level = arch_interrupt_disable();
    global_tick = tick;
    arch_interrupt_enable(level);
}

/**
 * This function will notify kernel there is one tick passed. Normally,
 * this function is invoked by clock ISR.
 */
void tick_increase(void)
{
    /* increase the global tick */
    ++global_tick;

    /* check time slice */
    Scheduler::inclock(global_tick);

    /* check timer */
    Timer::check();
}

/**
 * This function will calculate the tick from millisecond.
 *
 * @param ms the specified millisecond
 *
 * @return the calculated tick
 */
tick_t  tick_from_millisecond(uint32_t ms)
{
    /* return the calculated tick */
    return (CONFIG_TICK_PER_SECOND * ms + 999) / 1000;
}


/*@}*/

