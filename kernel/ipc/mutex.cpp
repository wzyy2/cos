/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "cos/ipc/mutex.h"
#include <cos/cos.h>


Mutex::Mutex(const char *name,
             uint8_t  flag):IPC(Object::Object_Class_Mutex, name)
{
    flag_ = flag;
    value_ = 1;
    owner_ = NULL;
    original_priority_ = 0xFF;
    hold_ = 0;
}

Mutex::~Mutex()
{

}

/**
 * This function will take a mutex, if the mutex is unavailable, the
 * thread shall wait for a specified time.
 *
 * @param mutex the mutex object
 * @param time the waiting time
 *
 * @return the error code
 */
err_t Mutex::take(int32_t time)
{
    register base_t temp;
    Thread *thread;

    /* this function must not be used in interrupt even if time = 0 */
    COS_DEBUG_IN_THREAD_CONTEXT;

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* get current thread */
    thread = Scheduler::get_current_thread();

    COS_DEBUG_LOG(COS_DEBUG_IPC,
                  ("mutex_take: current thread %s, mutex value: %d, hold: %d\n",
                   thread->name(), this->value_, this->hold_));

    /* reset thread error */
    thread->error_ = ERR_OK;

    if (this->owner_ == thread)
    {
        /* it's the same thread */
        this->hold_ ++;
    }
    else
    {
        /* The value of mutex is 1 in initial status. Therefore, if the
         * value is great than 0, it indicates the mutex is avaible.
         */
        if (this->value_ > 0)
        {
            /* mutex is available */
            this->value_ --;

            /* set mutex owner and original priority */
            this->owner_             = thread;
            this->original_priority_ = thread->current_priority_;
            this->hold_ ++;
        }
        else
        {
            /* no waiting, return with timeout */
            if (time == 0)
            {
                /* set error as timeout */
                thread->error_ = -ERR_TIMEOUT;

                /* enable interrupt */
                arch_interrupt_enable(temp);

                return -ERR_TIMEOUT;
            }
            else
            {
                /* mutex is unavailable, push to suspend list */
                COS_DEBUG_LOG(COS_DEBUG_IPC, ("mutex_take: suspend thread: %s\n",
                                            thread->name()));

                /* change the owner thread priority of mutex */
                if (thread->current_priority_ < this->owner_->current_priority_)
                {
                    /* change the owner thread priority */
                    this->owner_->control(Thread::CTRL_CHANGE_PRIORITY,
                                      &thread->current_priority_);
                }

                /* suspend current thread */
                list_suspend(thread);

                /* has waiting time, start thread timer */
                if (time > 0)
                {
                    COS_DEBUG_LOG(COS_DEBUG_IPC,
                                 ("mutex_take: start the timer of thread:%s\n",
                                  thread->name()));

                    /* reset the timeout of thread timer and start it */
                    thread->thread_timer_->control(Timer::CTRL_SET_TIME,
                                     &time);
                    thread->thread_timer_->start();
                }

                /* enable interrupt */
                arch_interrupt_enable(temp);

                /* do schedule */
                Scheduler::process();

                if (thread->error_ != ERR_OK)
                {
                    /* return error */
                    return thread->error_;
                }
                else
                {
                    /* the mutex is taken successfully. */
                    /* disable interrupt */
                    temp = arch_interrupt_disable();
                }
            }
        }
    }

    /* enable interrupt */
    arch_interrupt_enable(temp);

    return ERR_OK;
}


/**
 * This function will release a mutex, if there are threads suspended on mutex,
 * it will be waked up.
 *
 * @return the error code
 */
err_t Mutex::release()
{
    register base_t temp;
    Thread *thread;
    bool need_schedule;

    need_schedule = false;

    /* only thread could release mutex because we need test the ownership */
    COS_DEBUG_IN_THREAD_CONTEXT;

    /* get current thread */
    thread = Scheduler::get_current_thread();

    /* disable interrupt */
    temp = arch_interrupt_disable();

    COS_DEBUG_LOG(COS_DEBUG_IPC,
                 ("mutex_release:current thread %s, mutex value: %d, hold: %d\n",
                  thread->name(), this->value_, this->hold_));

    /* mutex only can be released by owner */
    if (thread != this->owner_)
    {
        thread->error_ = -ERR_ERROR;

        /* enable interrupt */
        arch_interrupt_enable(temp);

        return -ERR_ERROR;
    }

    /* decrease hold */
    this->hold_ --;
    /* if no hold */
    if (this->hold_ == 0)
    {
        /* change the owner thread to original priority */
        if (this->original_priority_ != this->owner_->current_priority_)
        {
            this->owner_->control(Thread::CTRL_CHANGE_PRIORITY,
                              &this->original_priority_);
        }

        /* wakeup suspended thread */
        if (!suspend_list_.empty())
        {
            /* get suspended thread */
            thread = suspend_list_.front();

            COS_DEBUG_LOG(COS_DEBUG_IPC, ("mutex_release: resume thread: %s\n",
                                        thread->name()));

            /* set new owner and priority */
            this->owner_             = thread;
            this->original_priority_ = thread->current_priority_;
            this->hold_ ++;

            /* resume thread */
            list_resume();

            need_schedule = true;
        }
        else
        {
            /* increase value */
            this->value_ ++;

            /* clear owner */
            this->owner_             = NULL;
            this->original_priority_ = 0xff;
        }
    }

    /* enable interrupt */
    arch_interrupt_enable(temp);

    /* perform a schedule */
    if (need_schedule == true)
        Scheduler::process();

    return ERR_OK;
}


/**
 * This function can get or set some extra attributions of a mutex object.
 *
 * @param cmd the execution command
 * @param arg the execution argument
 *
 * @return the error code
 */
err_t Mutex::control(uint8_t cmd, void *arg)
{
    return -ERR_ERROR;
}
