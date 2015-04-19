#include "cos/timer.h"

#include <cos/cos.h>

/**
 * @addtogroup Clock
 */

/*@{*/

coslib::RBTree<Timer> Timer::timer_tree_;

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
    flag_ &= ~Timer::FLAG_ACTIVATED;

    timeout_func_ = timeout;
    parameter_    = parameter;

    timeout_tick_ = 0;
    init_tick_    = time;
    node_ = new coslib::RBTree<Timer>::RBTreeNode (timeout_tick_, this, NULL);

}

Timer::~Timer()
{
    detach();

    delete node_;
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

    /* remove from set */
    timer_tree_.remove(node_);

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
    register base_t level;

    /* stop timer firstly */
    level = arch_interrupt_disable();
    /* remove timer from set */
    timer_tree_.remove(node_);
    /* change status of timer */
    flag_ &= ~Timer::FLAG_ACTIVATED;
    arch_interrupt_enable(level);

    /*
     * get timeout tick,
     * the max timeout tick shall not great than TICK_MAX/2
     */
    COS_ASSERT(init_tick_ < TICK_MAX / 2);
    timeout_tick_ = tick_get() + init_tick_;

    /* disable interrupt */
    level = arch_interrupt_disable();

    flag_ |= Timer::FLAG_ACTIVATED;

    node_->set_key(timeout_tick_);
    timer_tree_.insert(node_);

    /* enable interrupt */
    arch_interrupt_enable(level);

    return -ERR_OK;
}


/**
 * This function will stop the timer
 *
 * @return the operation status, ERR_OK on OK, -ERR_ERROR on error
 */
err_t Timer::stop()
{
    register base_t level;

    if (!(flag_ & Timer::FLAG_ACTIVATED))
        return -ERR_ERROR;

    /* disable interrupt */
    level = arch_interrupt_disable();

    timer_tree_.remove(node_);

    /* enable interrupt */
    arch_interrupt_enable(level);

    /* change stat */
    flag_ &= ~Timer::FLAG_ACTIVATED;

    return ERR_OK;
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
    case Timer::CTRL_GET_TIME:
        *(tick_t *)arg = init_tick_;
        break;

    case Timer::CTRL_SET_TIME:
        init_tick_ = *(tick_t *)arg;
        break;

    case Timer::CTRL_SET_ONESHOT:
        flag_ &= ~Timer::FLAG_PERIODIC;
        break;

    case Timer::CTRL_SET_PERIODIC:
        flag_ |= Timer::FLAG_PERIODIC;
        break;
    }

    return ERR_OK;
}


/**
 * This function will check timer set, if a timeout event happens, the
 * corresponding timeout function will be invoked.
 *
 * @note this function shall be invoked in operating system timer interrupt.
 */
void Timer::check(void)
{
    tick_t current_tick;
    register base_t level;

    COS_DEBUG_LOG(COS_DEBUG_TIMER, ("timer check enter\n"));

    current_tick = tick_get();

    /* disable interrupt */
    //level = arch_interrupt_disable();

    /* enter critical */
    //    Scheduler::enter_critical();

    while (!timer_tree_.empty())
    {
        Timer *t =  timer_tree_.min();

        /*
         * It supposes that the new tick shall less than the half duration of
         * tick max.
         */
        if ((current_tick - t->timeout_tick_) < TICK_MAX/2)
        {
            /* remove timer from timer set firstly */
            timer_tree_.remove(t->node_);


            /* call timeout function */
            t->timeout_func_(t->parameter_);

            /* re-get tick */
            current_tick = tick_get();

            COS_DEBUG_LOG(COS_DEBUG_TIMER, ("current tick: %d\n", current_tick));

            if ((t->flag_ & Timer::FLAG_PERIODIC) &&
                    (t->flag_ & Timer::FLAG_ACTIVATED))
            {
                /* start it */
                t->flag_ &= ~Timer::FLAG_ACTIVATED;
                t->start();
            }
            else
            {
                /* stop timer */
                t->flag_ &= ~Timer::FLAG_ACTIVATED;
            }

        }
        else
            break;
    }

    //    /* unlock scheduler */
    //    Scheduler::exit_critical();

    /* enable interrupt */
    //arch_inerrupt_enable(level);

    COS_DEBUG_LOG(COS_DEBUG_TIMER, ("timer check leave\n"));
}
/*@}*/
