/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "cos/ipc/event.h"
#include <cos/cos.h>

/**
 * This function will initialize an event and put it under control of resource
 * management.
 *
 * @param name the name of event
 * @param flag the flag of event
 */
Event::Event(const char *name,
             uint8_t  flag):IPC(Object::Object_Class_Event, name)
{
    flag_ = flag;
    set_ = 0;
}

Event::~Event()
{

}

/**
 * This function will send an event to the event object, if there are threads
 * suspended on event object, it will be waked up.
 *
 * @param set the event set
 *
 * @return the error code
 */
err_t Event::send(uint32_t set)
{
    Thread *thread;
    register ubase_t level;
    register base_t status;
    bool need_schedule;

    /* parameter check */
    if (set == 0)
        return -ERR_ERROR;

    need_schedule = false;

    /* disable interrupt */
    level = arch_interrupt_disable();

    /* set event */
    this->set_ |= set;

    if ((!suspend_list_.empty())) {
        /* search thread list to resume thread */
        auto node = suspend_list_.begin();

        while(node != NULL) {
            /* get thread */
            thread = node->getValue();

            status = -ERR_ERROR;
            if (thread->event_info_ & FLAG_AND)
            {
                if ((thread->event_set_ & this->set_) == thread->event_set_)
                {
                    /* received an AND event */
                    status = ERR_OK;
                }
            }
            else if (thread->event_info_ & FLAG_OR)
            {
                if (thread->event_set_ & this->set_)
                {
                    /* save recieved event set */
                    thread->event_set_ = thread->event_set_ & this->set_;

                    /* received an OR event */
                    status = ERR_OK;
                }
            }

            /* move node to the next */
            node = node->getNext();

            /* condition is satisfied, resume thread */
            if (status == ERR_OK)
            {
                /* clear event */
                if (thread->event_info_ & FLAG_CLEAR)
                    this->set_ &= ~thread->event_set_;

                /* resume thread, and thread list breaks out */
                suspend_list_.erase(thread->list_node_);
                thread->resume();

                /* need do a scheduling */
                need_schedule = true;
            }

        }
    }

    /* enable interrupt */
    arch_interrupt_enable(level);

    /* do a schedule */
    if (need_schedule == true)
        Scheduler::process();

    return ERR_OK;
}

/**
 * This function will receive an event from event object, if the event is
 * unavailable, the thread shall wait for a specified time.
 *
 * @param set the interested event set
 * @param option the receive option, either FLAG_AND or
 *         FLAG_OR should be set.
 * @param timeout the waiting time
 * @param recved the received event
 *
 * @return the error code
 */
err_t Event::recv(uint32_t  set,
                       uint8_t   option,
                       int32_t   timeout,
                       uint32_t *recved)
{
    Thread *thread;
    register ubase_t level;
    register base_t status;

    /* parameter check */
    if (set == 0)
        return -ERR_ERROR;

    /* init status */
    status = -ERR_ERROR;
    /* get current thread */
    thread = Scheduler::get_current_thread();
    /* reset thread error */
    thread->error_ = ERR_OK;

    /* disable interrupt */
    level = arch_interrupt_disable();

    /* check event set */
    if (option & FLAG_AND)
    {
        if ((this->set_ & set) == set)
            status = ERR_OK;
    }
    else if (option & FLAG_OR)
    {
        if (this->set_ & set)
            status = ERR_OK;
    }
    else
    {
        /* either FLAG_AND or FLAG_OR should be set */
        COS_ASSERT(0);
    }

    if (status == ERR_OK)
    {
        /* set received event */
        *recved = (this->set_ & set);

        /* received event */
        if (option & FLAG_CLEAR)
            this->set_ &= ~set;
    }
    else if (timeout == 0)
    {
        /* no waiting */
        thread->error_ = -ERR_TIMEOUT;
    }
    else
    {
        /* fill thread event info */
        thread->event_set_  = set;
        thread->event_info_ = option;

        /* put thread to suspended thread list */
        list_suspend(thread);

        /* if there is a waiting timeout, active thread timer */
        if (timeout > 0)
        {
            /* reset the timeout of thread timer and start it */
            thread->thread_timer_->control(Timer::CTRL_SET_TIME,
                             &timeout);
            thread->thread_timer_->start();
        }

        /* enable interrupt */
        arch_interrupt_enable(level);

        /* do a schedule */
        Scheduler::process();

        if (thread->error_ != ERR_OK)
        {
            /* return error */
            return thread->error_;
        }

        /* received an event, disable interrupt to protect */
        level = arch_interrupt_disable();

        /* set received event */
        *recved = thread->event_set_;
    }

    /* enable interrupt */
    arch_interrupt_enable(level);

    return thread->error_;
}

/**
 * This function can get or set some extra attributions of an event object.
 *
 * @param cmd the execution command
 * @param arg the execution argument
 *
 * @return the error code
 */
err_t Event::control(uint8_t cmd, void *arg)
{
    ubase_t level;

    if (cmd == CMD_RESET)
    {
        /* disable interrupt */
        level = arch_interrupt_disable();

        /* resume all waiting thread */
        list_resume_all();

        /* init event set */
        this->set_ = 0;

        /* enable interrupt */
        arch_interrupt_enable(level);

        Scheduler::process();

        return ERR_OK;
    }

    return -ERR_ERROR;
}
