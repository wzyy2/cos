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
MailBox::MailBox(const char *name,
                 void        *msgpool,
                 size_t    size,
                 uint8_t  flag):IPC(Object::Object_Class_Event, name)
{
    flag_ = flag;
    msg_pool_ = (uint32_t *) msgpool;
    size_ = size;
    entry_ = 0;
    in_offset_ = 0;
    out_offset_ = 0;
}

MailBox::MailBox(const char *name,
                 size_t    size,
                 uint8_t  flag):IPC(Object::Object_Class_Event, name)
{
    flag_ = flag;
    msg_pool_ = (uint32_t *) kmalloc(size);

    if(msg_pool_ != NULL) {
        delete_pool_flag_ = true;
    } else {
        set_errno(-ERR_ERROR);
        printk("ERROR : malloc msg_pool failed.\n");
    }


    size_ = size;
    entry_ = 0;
    in_offset_ = 0;
    out_offset_ = 0;
}

MailBox::~MailBox()
{
    resume_all();

    if(delete_pool_flag_)
        kfree(msg_pool_);
}

err_t MailBox::resume_all()
{
    Thread *thread;
    register ubase_t temp;

    /* wakeup all suspend threads */
    while (!suspend_sender_thread_.empty())
    {
        /* disable interrupt */
        temp = arch_interrupt_disable();

        /* get next suspend thread */
        thread = suspend_sender_thread_.front();
        /* set error code to ERR_ERROR */
        thread->error_ = -ERR_ERROR;

        suspend_sender_thread_.erase(thread->list_node_);

        thread->resume();

        /* enable interrupt */
        arch_interrupt_enable(temp);
    }

    return ERR_OK;
}

/**
 * This function will send a mail to mailbox object. If the mailbox is full,
 * current thread will be suspended until timeout.
 *
 * @param value the mail
 * @param timeout the waiting time
 *
 * @return the error code
 */
err_t MailBox::send_wait(uint32_t  value,
                         int32_t   timeout)
{
    Thread *thread;
    register ubase_t temp;
    uint32_t tick_delta;

    /* initialize delta tick */
    tick_delta = 0;
    /* get current thread */
    thread = Scheduler::get_current_thread();

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* for non-blocking call */
    if (this->entry_ == this->size_ && timeout == 0)
    {
        arch_interrupt_enable(temp);

        return -ERR_FULL;
    }

    /* mailbox is full */
    while (this->entry_ == this->size_)
    {
        /* reset error number in thread */
        thread->error_ = ERR_OK;

        /* no waiting, return timeout */
        if (timeout == 0)
        {
            /* enable interrupt */
            arch_interrupt_enable(temp);

            return -ERR_FULL;
        }

        COS_DEBUG_IN_THREAD_CONTEXT;
        /* suspend current thread */
        thread->suspend();
        suspend_sender_thread_.push_back(thread->list_node_);


        /* has waiting time, start thread timer */
        if (timeout > 0)
        {
            /* get the start tick of timer */
            tick_delta = tick_get();

            COS_DEBUG_LOG(COS_DEBUG_IPC, ("send_wait: start timer of thread:%s\n",
                                        thread->name()));

            /* reset the timeout of thread timer and start it */
            thread->thread_timer_->control(Timer::CTRL_SET_TIME,
                             &timeout);
            thread->thread_timer_->start();

        }

        /* enable interrupt */
        arch_interrupt_enable(temp);

        /* re-schedule */
        Scheduler::process();

        /* resume from suspend state */
        if (thread->error_ != ERR_OK)
        {
            /* return error */
            return thread->error_;
        }

        /* disable interrupt */
        temp = arch_interrupt_disable();

        /* if it's not waiting forever and then re-calculate timeout tick */
        if (timeout > 0)
        {
            tick_delta = tick_get() - tick_delta;
            timeout -= tick_delta;
            if (timeout < 0)
                timeout = 0;
        }
    }

    /* set ptr */
    this->msg_pool_[this->in_offset_] = value;
    /* increase input offset */
    ++ this->in_offset_;
    if (this->in_offset_ >= this->size_)
        this->in_offset_ = 0;
    /* increase message entry */
    this->entry_ ++;

    /* resume suspended thread */
    if (list_resume() == ERR_OK)
    {
        /* enable interrupt */
        arch_interrupt_enable(temp);

        Scheduler::process();

        return ERR_OK;
    }

    /* enable interrupt */
    arch_interrupt_enable(temp);

    return ERR_OK;
}

/**
 * This function will send a mail to mailbox object, if there are threads
 * suspended on mailbox object, it will be waked up. This function will return
 * immediately, if you want blocking send, use send_wait instead.
 *
 * @param value the mail
 *
 * @return the error code
 */
err_t MailBox::send(uint32_t value)
{
    return send_wait(value, 0);
}

/**
 * This function will receive a mail from mailbox object, if there is no mail
 * in mailbox object, the thread shall wait for a specified time.
 *
 * @param value the received mail will be saved in
 * @param timeout the waiting time
 *
 * @return the error code
 */
err_t MailBox::recv(uint32_t *value, int32_t timeout)
{
    Thread *thread;
    register ubase_t temp;
    uint32_t tick_delta;

    /* initialize delta tick */
    tick_delta = 0;
    /* get current thread */
    thread = Scheduler::get_current_thread();

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* for non-blocking call */
    if (this->entry_ == 0 && timeout == 0)
    {
        arch_interrupt_enable(temp);

        return -ERR_TIMEOUT;
    }

    /* mailbox is empty */
    while (this->entry_ == 0)
    {
        /* reset error number in thread */
        thread->error_ = ERR_OK;

        /* no waiting, return timeout */
        if (timeout == 0)
        {
            /* enable interrupt */
            arch_interrupt_enable(temp);

            thread->error_ = -ERR_TIMEOUT;

            return -ERR_TIMEOUT;
        }

        /* suspend current thread */
        list_suspend(thread);

        /* has waiting time, start thread timer */
        if (timeout > 0)
        {
            /* get the start tick of timer */
            tick_delta = tick_get();

            COS_DEBUG_LOG(COS_DEBUG_IPC, ("mb_recv: start timer of thread:%s\n",
                                          thread->name()));

            /* reset the timeout of thread timer and start it */
            thread->thread_timer_->control(Timer::CTRL_SET_TIME,
                                           &timeout);
            thread->thread_timer_->start();
        }

        /* enable interrupt */
        arch_interrupt_enable(temp);

        /* re-schedule */
        Scheduler::process();

        /* resume from suspend state */
        if (thread->error_ != ERR_OK)
        {
            /* return error */
            return thread->error_;
        }

        /* disable interrupt */
        temp = arch_interrupt_disable();

        /* if it's not waiting forever and then re-calculate timeout tick */
        if (timeout > 0)
        {
            tick_delta = tick_get() - tick_delta;
            timeout -= tick_delta;
            if (timeout < 0)
                timeout = 0;
        }
    }

    /* fill ptr */
    *value = this->msg_pool_[this->out_offset_];

    /* increase output offset */
    ++ this->out_offset_;
    if (this->out_offset_ >= this->size_)
        this->out_offset_ = 0;
    /* decrease message entry */
    this->entry_ --;

    /* resume suspended thread */
    if (!suspend_sender_thread_.empty())
    {
        Thread *thread = suspend_sender_thread_.front();

        suspend_sender_thread_.erase(thread->list_node_);

        thread->resume();

        /* enable interrupt */
        arch_interrupt_enable(temp);

        Scheduler::process();

        return ERR_OK;
    }

    /* enable interrupt */
    arch_interrupt_enable(temp);

    return ERR_OK;
}


/**
 * This function can get or set some extra attributions of a mailbox object.
 *
 * @param cmd the execution command
 * @param arg the execution argument
 *
 * @return the error code
 */
err_t MailBox::control(uint8_t cmd, void *arg)
{
    ubase_t level;

    if (cmd == CMD_RESET)
    {
        /* disable interrupt */
        level = arch_interrupt_disable();

        /* resume all waiting thread */
        IPC::list_resume_all();
        /* also resume all mailbox private suspended thread */
        MailBox::resume_all();

        /* re-init mailbox */
        this->entry_      = 0;
        this->in_offset_  = 0;
        this->out_offset_ = 0;

        /* enable interrupt */
        arch_interrupt_enable(level);

        Scheduler::process();

        return ERR_OK;
    }

    return -ERR_ERROR;
}
