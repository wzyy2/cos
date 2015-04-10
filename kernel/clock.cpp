#include <cos/clock.h>
#include <cos/cos.h>
#include <cos/cosHw.h>

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
//    struct rt_thread *thread;

    /* increase the global tick */
    ++ global_tick;

    /* check time slice */
//    thread = rt_thread_self();

//    -- thread->remaining_tick;
//    if (thread->remaining_tick == 0)
//    {
//        /* change to initialized tick */
//        thread->remaining_tick = thread->init_tick;

//        /* yield */
//        rt_thread_yield();
//    }

    /* check timer */
//    rt_timer_check();
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

