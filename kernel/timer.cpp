#include "cos/timer.h"


/**
 * @addtogroup Clock
 */

/*@{*/
/**
 * This function will initialize a timer, normally this function is used to
 * initialize a static timer object.
 *
 * @param name the name of timer
 * @param timeout the timeout function
 * @param parameter the parameter of timeout function
 * @param time the tick of timer
 * @param flag the flag of timer
 */
Timer::Timer(const char *name,
             void (*timeout)(void *parameter),
             void      *parameter,
             tick_t  time,
             uint8_t flag):Object(Object::Object_Class_Timer, name)
{
    /* set flag */
    flag_ = flag;

    /* set deactivated */
    flag_ &= ~TIMER_FLAG_ACTIVATED;

    timeout_func_ = timeout;
    parameter_    = parameter;

    timeout_tick_ = 0;
    init_tick_    = time;

    /* initialize timer list */

}

Timer::~Timer()
{
    detach();
}

/**
 * This function will detach a timer from timer management.
 *
 * @return the operation status, ERR_OK on OK; ERR_ERROR on error
 */
err_t Timer::detach()
{
    register    base_t level;

    /* disable interrupt */
    level = arch_interrupt_disable();

    //_rt_timer_remove(timer);

    /* enable interrupt */
    arch_interrupt_enable(level);

    Object::detach();

    return -ERR_OK;
}


/**
 * This function will start the timer
 *
 * @return the operation status, ERR_OK on OK, -ERR_ERROR on error
 */
err_t Timer::start()
{
    int row_lvl;
    rt_list_t *timer_list;
    register base_t level;
    rt_list_t *row_head[RT_TIMER_SKIP_LIST_LEVEL];
    unsigned int tst_nr;
    static unsigned int random_nr;

    /* stop timer firstly */
    level = arch_interrupt_disable();
    /* remove timer from list */
    _rt_timer_remove(timer);
    /* change status of timer */
    flag_ &= ~TIMER_FLAG_ACTIVATED;
    arch_interrupt_enable(level);

    /*
     * get timeout tick,
     * the max timeout tick shall not great than RT_TICK_MAX/2
     */
    RT_ASSERT(timer->init_tick < RT_TICK_MAX / 2);
    timer->timeout_tick = rt_tick_get() + timer->init_tick;

    /* disable interrupt */
    level = arch_interrupt_disable();

#ifdef RT_USING_TIMER_SOFT
    if (timer->parent.flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        /* insert timer to soft timer list */
        timer_list = rt_soft_timer_list;
    }
    else
#endif
    {
        /* insert timer to system timer list */
        timer_list = rt_timer_list;
    }

    row_head[0]  = &timer_list[0];
    for (row_lvl = 0; row_lvl < RT_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        for (;row_head[row_lvl] != timer_list[row_lvl].prev;
             row_head[row_lvl]  = row_head[row_lvl]->next)
        {
            struct rt_timer *t;
            rt_list_t *p = row_head[row_lvl]->next;

            /* fix up the entry pointer */
            t = rt_list_entry(p, struct rt_timer, row[row_lvl]);

            /* If we have two timers that timeout at the same time, it's
             * preferred that the timer inserted early get called early.
             * So insert the new timer to the end the the some-timeout timer
             * list.
             */
            if ((t->timeout_tick - timer->timeout_tick) == 0)
            {
                continue;
            }
            else if ((t->timeout_tick - timer->timeout_tick) < RT_TICK_MAX / 2)
            {
                break;
            }
        }
        if (row_lvl != RT_TIMER_SKIP_LIST_LEVEL - 1)
            row_head[row_lvl+1] = row_head[row_lvl]+1;
    }

    /* Interestingly, this super simple timer insert counter works very very
     * well on distributing the list height uniformly. By means of "very very
     * well", I mean it beats the randomness of timer->timeout_tick very easily
     * (actually, the timeout_tick is not random and easy to be attacked). */
    random_nr++;
    tst_nr = random_nr;

    rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL-1],
                         &(timer->row[RT_TIMER_SKIP_LIST_LEVEL-1]));
    for (row_lvl = 2; row_lvl <= RT_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        if (!(tst_nr & RT_TIMER_SKIP_LIST_MASK))
            rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL - row_lvl],
                                 &(timer->row[RT_TIMER_SKIP_LIST_LEVEL - row_lvl]));
        else
            break;
        /* Shift over the bits we have tested. Works well with 1 bit and 2
         * bits. */
        tst_nr >>= (RT_TIMER_SKIP_LIST_MASK+1)>>1;
    }

    timer->parent.flag |= RT_TIMER_FLAG_ACTIVATED;

    /* enable interrupt */
    arch_interrupt_enable(level);

#ifdef RT_USING_TIMER_SOFT
    if (timer->parent.flag & RT_TIMER_FLAG_SOFT_TIMER)
    {
        /* check whether timer thread is ready */
        if (timer_thread.stat != RT_THREAD_READY)
        {
            /* resume timer thread to check soft timer */
            rt_thread_resume(&timer_thread);
            rt_schedule();
        }
    }
#endif

    return -RT_EOK;
}


/**
 * This function will stop the timer
 *
 * @return the operation status, ERR_OK on OK, -ERR_ERROR on error
 */
err_t Timer::stop()
{
    register base_t level;

    /* timer check */
    RT_ASSERT(timer != RT_NULL);
    if (!(timer->parent.flag & RT_TIMER_FLAG_ACTIVATED))
        return -RT_ERROR;

    RT_OBJECT_HOOK_CALL(rt_object_put_hook, (&(timer->parent)));

    /* disable interrupt */
    level = arch_interrupt_disable();

    _rt_timer_remove(timer);

    /* enable interrupt */
    arch_interrupt_enable(level);

    /* change stat */
    timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;

    return RT_EOK;
}


/**
 * This function will get or set some options of the timer
 *
 * @param cmd the control command
 * @param arg the argument
 *
 * @return ERR_EOK
 */
err_t Timer::control(uint8_t cmd, void *arg)
{
    switch (cmd)
    {
    case RT_TIMER_CTRL_GET_TIME:
        *(tick_t *)arg = timer->init_tick;
        break;

    case RT_TIMER_CTRL_SET_TIME:
        timer->init_tick = *(tick_t *)arg;
        break;

    case RT_TIMER_CTRL_SET_ONESHOT:
        timer->parent.flag &= ~RT_TIMER_FLAG_PERIODIC;
        break;

    case RT_TIMER_CTRL_SET_PERIODIC:
        timer->parent.flag |= RT_TIMER_FLAG_PERIODIC;
        break;
    }

    return RT_EOK;
}


/**
 * This function will check timer list, if a timeout event happens, the
 * corresponding timeout function will be invoked.
 *
 * @note this function shall be invoked in operating system timer interrupt.
 */
void Timer::check(void)
{
    struct rt_timer *t;
    tick_t current_tick;
    register base_t level;

    COS_DEBUG_LOG(COS_DEBUG_TIMER, ("timer check enter\n"));

    current_tick = rt_tick_get();

    /* disable interrupt */
    level = arch_interrupt_disable();

    while (!rt_list_isempty(&rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL-1]))
    {
        t = rt_list_entry(rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL - 1].next,
                          struct rt_timer, row[RT_TIMER_SKIP_LIST_LEVEL - 1]);

        /*
         * It supposes that the new tick shall less than the half duration of
         * tick max.
         */
        if ((current_tick - t->timeout_tick) < RT_TICK_MAX/2)
        {
            RT_OBJECT_HOOK_CALL(rt_timer_timeout_hook, (t));

            /* remove timer from timer list firstly */
            _rt_timer_remove(t);

            /* call timeout function */
            t->timeout_func(t->parameter);

            /* re-get tick */
            current_tick = rt_tick_get();

            COS_DEBUG_LOG(COS_DEBUG_TIMER, ("current tick: %d\n", current_tick));

            if ((t->parent.flag & RT_TIMER_FLAG_PERIODIC) &&
                (t->parent.flag & RT_TIMER_FLAG_ACTIVATED))
            {
                /* start it */
                t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
                rt_timer_start(t);
            }
            else
            {
                /* stop timer */
                t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
            }
        }
        else
            break;
    }

    /* enable interrupt */
    arch_interrupt_enable(level);

    COS_DEBUG_LOG(COS_DEBUG_TIMER, ("timer check leave\n"));
}

/**
 * This function will return the next timeout tick in the system.
 *
 * @return the next timeout tick in the system
 */
tick_t Timer::next_timeout_tick(void)
{
    return rt_timer_list_next_timeout(rt_timer_list);
}

#ifdef RT_USING_TIMER_SOFT
/**
 * This function will check timer list, if a timeout event happens, the
 * corresponding timeout function will be invoked.
 */
void rt_soft_timer_check(void)
{
    tick_t current_tick;
    rt_list_t *n;
    struct rt_timer *t;

    COS_DEBUG_LOG(COS_DEBUG_TIMER, ("software timer check enter\n"));

    current_tick = rt_tick_get();

    /* lock scheduler */
    rt_enter_critical();

    for (n = rt_soft_timer_list[RT_TIMER_SKIP_LIST_LEVEL-1].next;
         n != &(rt_soft_timer_list[RT_TIMER_SKIP_LIST_LEVEL-1]);)
    {
        t = rt_list_entry(n, struct rt_timer, row[RT_TIMER_SKIP_LIST_LEVEL-1]);

        /*
         * It supposes that the new tick shall less than the half duration of
         * tick max.
         */
        if ((current_tick - t->timeout_tick) < RT_TICK_MAX / 2)
        {
            RT_OBJECT_HOOK_CALL(rt_timer_timeout_hook, (t));

            /* move node to the next */
            n = n->next;

            /* remove timer from timer list firstly */
            _rt_timer_remove(t);

            /* not lock scheduler when performing timeout function */
            rt_exit_critical();
            /* call timeout function */
            t->timeout_func(t->parameter);

            /* re-get tick */
            current_tick = rt_tick_get();

            COS_DEBUG_LOG(COS_DEBUG_TIMER, ("current tick: %d\n", current_tick));

            /* lock scheduler */
            rt_enter_critical();

            if ((t->parent.flag & RT_TIMER_FLAG_PERIODIC) &&
                (t->parent.flag & RT_TIMER_FLAG_ACTIVATED))
            {
                /* start it */
                t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
                rt_timer_start(t);
            }
            else
            {
                /* stop timer */
                t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
            }
        }
        else break; /* not check anymore */
    }

    /* unlock scheduler */
    rt_exit_critical();

    COS_DEBUG_LOG(COS_DEBUG_TIMER, ("software timer check leave\n"));
}

/* system timer thread entry */
static void rt_thread_timer_entry(void *parameter)
{
    tick_t next_timeout;

    while (1)
    {
        /* get the next timeout tick */
        next_timeout = rt_timer_list_next_timeout(rt_soft_timer_list);
        if (next_timeout == RT_TICK_MAX)
        {
            /* no software timer exist, suspend self. */
            rt_thread_suspend(rt_thread_self());
            rt_schedule();
        }
        else
        {
            tick_t current_tick;

            /* get current tick */
            current_tick = rt_tick_get();

            if ((next_timeout - current_tick) < RT_TICK_MAX/2)
            {
                /* get the delta timeout tick */
                next_timeout = next_timeout - current_tick;
                rt_thread_delay(next_timeout);
            }
        }

        /* check software timer */
        rt_soft_timer_check();
    }
}
#endif

/*@}*/
