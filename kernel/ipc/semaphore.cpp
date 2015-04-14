#include "cos/semaphore.h"
#include <cos/cos.h>


/**
 * This function will initialize a semaphore and put it under control of
 * resource management.
 *
 * @param name the name of semaphore
 * @param value the init value of semaphore
 * @param flag the flag of semaphore
 *
 * @return the operation status, ERR_OK on successful
 */
Semaphore::Semaphore(const char *name,
                     uint32_t value,
                     uint8_t  flag):IPC(Object::Object_Class_Semaphore, name)
{
    flag_ = flag;
    value_ = value;
}

Semaphore::~Semaphore()
{
    detach();
}

/**
 * This function will detach a semaphore from resource management
 *
 * @return the operation status, ERR_OK on successful
 */
err_t Semaphore::detach()
{
    /* wakeup all suspend threads */
    //rt_ipc_list_resume_all(&(sem->parent.suspend_thread));

    /* detach semaphore object */
    Object::detach();

    return ERR_OK;
}


/**
 * This function will take a semaphore, if the semaphore is unavailable, the
 * thread shall wait for a specified time.
 *
 * @param time the waiting time
 *
 * @return the error code
 */
err_t Semaphore::take(int32_t time)
{
    register base_t temp;

    /* disable interrupt */
    temp = arch_interrupt_disable();

    COS_DEBUG_LOG(COS_DEBUG_IPC, ("thread %s take sem:%s, which value is: %d\n",
                                Scheduler::get_current_thread()->name().c_str(),
                                name_.c_str(),
                                value_));

    if (value_ > 0)
    {
        /* semaphore is available */
        value_ --;

        /* enable interrupt */
        arch_interrupt_enable(temp);
    }
    else
    {
        /* no waiting, return with timeout */
        if (time == 0)
        {
            arch_interrupt_enable(temp);

            return -ERR_TIMEOUT;
        }
        else
        {
            /* current context checking */
            COS_DEBUG_IN_THREAD_CONTEXT;

            /* semaphore is unavailable, push to suspend list */
            /* get current thread */
            Thread *thread = Scheduler::get_current_thread();

            /* reset thread error number */
            set_errno(ERR_OK);

            COS_DEBUG_LOG(COS_DEBUG_IPC, ("sem take: suspend thread - %s\n",
                                        thread->name().c_str()));

            /* suspend thread */
//            rt_ipc_list_suspend(&(sem->parent.suspend_thread),
//                                thread,
//                                sem->parent.parent.flag);

            /* has waiting time, start thread timer */
            if (time > 0)
            {
                COS_DEBUG_LOG(COS_DEBUG_IPC, ("set thread:%s to timer list\n",
                                            thread->name().c_str()));

                /* reset the timeout of thread timer and start it */
//                rt_timer_control(&(thread->thread_timer),
//                                 RT_TIMER_CTRL_SET_TIME,
//                                 &time);
//                rt_timer_start(&(thread->thread_timer));
            }

            /* enable interrupt */
            arch_interrupt_enable(temp);

            /* do schedule */
//            rt_schedule();

//            if (thread->error != ERR_OK)
//            {
//                return thread->error;
//            }
        }
    }

    return ERR_OK;
}

/**
 * This function will try to take a semaphore and immediately return
 *
 * @return the error code
 */
err_t Semaphore::trytake()
{
    return take(0);
}

/**
 * This function will release a semaphore, if there are threads suspended on
 * semaphore, it will be waked up.
 *
 * @return the error code
 */
err_t Semaphore::release()
{
    register base_t temp;
    register bool need_schedule;

    need_schedule = false;

    /* disable interrupt */
    temp = arch_interrupt_disable();

    COS_DEBUG_LOG(COS_DEBUG_IPC, ("thread %s releases sem:%s, which value is: %d\n",
                                Scheduler::get_current_thread()->name().c_str(),
                                name_.c_str(),
                                value_));

//    if (!rt_list_isempty(&sem->parent.suspend_thread))
//    {
//        /* resume the suspended thread */
//        rt_ipc_list_resume(&(sem->parent.suspend_thread));
//        need_schedule = RT_TRUE;
//    }
//    else
        value_ ++; /* increase value */

    /* enable interrupt */
    arch_interrupt_enable(temp);

    /* resume a thread, re-schedule */
//    if (need_schedule == RT_TRUE)
//        rt_schedule();

    return ERR_OK;
}


/**
 * This function can get or set some extra attributions of a semaphore object.
 *
 * @param cmd the execution command
 * @param arg the execution argument
 *
 * @return the error code
 */
err_t Semaphore::control(uint8_t cmd, void *arg)
{
    ubase_t level;

    if (cmd == IPC::CMD_RESET)
    {
        uint32_t value;

        /* get value */
        value = (uint32_t)arg;
        /* disable interrupt */
        level = arch_interrupt_disable();

        /* resume all waiting thread */
        //rt_ipc_list_resume_all(&sem->parent.suspend_thread);

        /* set new value */
        value_ = (uint16_t)value;

        /* enable interrupt */
        arch_interrupt_enable(level);

        //rt_schedule();

        return ERR_OK;
    }

    return -ERR_ERROR;
}
