/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "cos/ipc/messagequeue.h"
#include <cos/cos.h>

/**
 * This function will initialize an event and put it under control of resource
 * management.
 *
 * @param name the name of event
 * @param flag the flag of event
 */
MessageQueue::MessageQueue(const char *name,
                           size_t   msg_size,
                           size_t   pool_size,
                           uint8_t  flag):IPC(Object::Object_Class_MessageQueue, name)
{
    struct mq_message *head;
    register base_t temp;

    /* set parent flag */
    flag_ = flag;

    /* set messasge pool */
    this->msg_pool_ = kmalloc(msg_size);

    if(msg_pool_ == NULL) {
        set_errno(-ERR_ERROR);
        printk("ERROR : malloc pool failed.\n");
    }

    /* get correct message size */
    this->msg_size_ = WIDTH_ALIGN(msg_size, CONFIG_ALIGN_SIZE);
    this->max_msgs_ = pool_size / (this->msg_size_ + sizeof(struct mq_message));

    /* init message list */
    this->msg_queue_head_ = NULL;
    this->msg_queue_tail_ = NULL;

    /* init message empty list */
    this->msg_queue_free_ = NULL;
    for (temp = 0; temp < this->max_msgs_; temp ++)
    {
        head = (struct mq_message *)((uint8_t *)this->msg_pool_ +
                                     temp * (this->msg_size_ + sizeof(struct mq_message)));
        head->next = (mq_message *) this->msg_queue_free_;
        this->msg_queue_free_ = head;
    }

    /* the initial entry is zero */
    this->entry_ = 0;

}

MessageQueue::~MessageQueue()
{
    kfree(msg_pool_);
}

/**
 * This function will send a message to message queue object, if there are
 * threads suspended on message queue object, it will be waked up.
 *
 * @param buffer the message
 * @param size the size of buffer
 *
 * @return the error code
 */
err_t MessageQueue::send(void *buffer, size_t size)
{
    register ubase_t temp;
    struct mq_message *msg;

    COS_ASSERT(buffer != NULL);
    COS_ASSERT(size != 0);

    /* greater than one message size */
    if (size > this->msg_size_)
        return -ERR_ERROR;

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* get a free list, there must be an empty item */
    msg = (struct mq_message*)this->msg_queue_free_;
    /* message queue is full */
    if (msg == NULL)
    {
        /* enable interrupt */
        arch_interrupt_enable(temp);

        return -ERR_FULL;
    }
    /* move free list pointer */
    this->msg_queue_free_ = msg->next;

    /* enable interrupt */
    arch_interrupt_enable(temp);

    /* the msg is the new tailer of list, the next shall be NULL */
    msg->next = NULL;
    /* copy buffer */
    memcpy(msg + 1, buffer, size);

    /* disable interrupt */
    temp = arch_interrupt_disable();
    /* link msg to message queue */
    if (this->msg_queue_tail_ != NULL)
    {
        /* if the tail exists, */
        ((struct mq_message *)this->msg_queue_tail_)->next = msg;
    }

    /* set new tail */
    this->msg_queue_tail_ = msg;
    /* if the head is empty, set head */
    if (this->msg_queue_head_ == NULL)
        this->msg_queue_head_ = msg;

    /* increase message entry */
    this->entry_ ++;

    /* resume suspended thread */
    if (!suspend_list_.empty())
    {
        /* get suspended thread */
        list_resume();

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
 * This function will send an urgent message to message queue object, which
 * means the message will be inserted to the head of message queue. If there
 * are threads suspended on message queue object, it will be waked up.
 *
 * @param buffer the message
 * @param size the size of buffer
 *
 * @return the error code
 */
err_t MessageQueue::urgent(void *buffer, size_t size)
{
    register ubase_t temp;
    struct mq_message *msg;

    COS_ASSERT(buffer != NULL);
    COS_ASSERT(size != 0);

    /* greater than one message size */
    if (size > this->msg_size_)
        return -ERR_ERROR;

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* get a free list, there must be an empty item */
    msg = (struct mq_message *)this->msg_queue_free_;
    /* message queue is full */
    if (msg == NULL)
    {
        /* enable interrupt */
        arch_interrupt_enable(temp);

        return -ERR_FULL;
    }
    /* move free list pointer */
    this->msg_queue_free_ = msg->next;

    /* enable interrupt */
    arch_interrupt_enable(temp);

    /* copy buffer */
    memcpy(msg + 1, buffer, size);

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* link msg to the beginning of message queue */
    msg->next = (mq_message *) this->msg_queue_head_;
    this->msg_queue_head_ = msg;

    /* if there is no tail */
    if (this->msg_queue_tail_ == NULL)
        this->msg_queue_tail_ = msg;

    /* increase message entry */
    this->entry_ ++;

    /* resume suspended thread */
    if (!suspend_list_.empty())
    {
        /* get suspended thread */
        list_resume();

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
 * This function will receive a message from message queue object, if there is
 * no message in message queue object, the thread shall wait for a specified
 * time.
 *
 * @param buffer the received message will be saved in
 * @param size the size of buffer
 * @param timeout the waiting time
 *
 * @return the error code
 */
err_t MessageQueue::recv(void      *buffer,
                         size_t  size,
                         int32_t timeout)
{
    Thread *thread;
    register ubase_t temp;
    struct mq_message *msg;
    uint32_t tick_delta;

    COS_ASSERT(buffer != NULL);
    COS_ASSERT(size != 0);

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

    /* message queue is empty */
    while (this->entry_ == 0)
    {
        COS_DEBUG_IN_THREAD_CONTEXT;

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

            COS_DEBUG_LOG(COS_DEBUG_IPC, ("set thread:%s to timer list\n",
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

        /* recv message */
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

    /* get message from queue */
    msg = (struct mq_message *)this->msg_queue_head_;

    /* move message queue head */
    this->msg_queue_head_ = msg->next;
    /* reach queue tail, set to NULL */
    if (this->msg_queue_tail_ == msg)
        this->msg_queue_tail_ = NULL;

    /* decrease message entry */
    this->entry_ --;

    /* enable interrupt */
    arch_interrupt_enable(temp);

    /* copy message */
    memcpy(buffer, msg + 1, size > this->msg_size_ ? this->msg_size_ : size);

    /* disable interrupt */
    temp = arch_interrupt_disable();
    /* put message to free list */
    msg->next = (struct mq_message *)this->msg_queue_free_;
    this->msg_queue_free_ = msg;
    /* enable interrupt */
    arch_interrupt_enable(temp);

    return ERR_OK;
}


/**
 * This function can get or set some extra attributions of a message queue
 * object.
 *
 * @param cmd the execution command
 * @param arg the execution argument
 *
 * @return the error code
 */
err_t MessageQueue::control(uint8_t cmd, void *arg)
{
    ubase_t level;
    struct mq_message *msg;

    if (cmd == IPC::CMD_RESET)
    {
        /* disable interrupt */
        level = arch_interrupt_disable();

        /* resume all waiting thread */
        IPC::list_resume_all();

        /* release all message in the queue */
        while (this->msg_queue_head_ != NULL)
        {
            /* get message from queue */
            msg = (mq_message *)this->msg_queue_head_;

            /* move message queue head */
            this->msg_queue_head_ = msg->next;
            /* reach queue tail, set to NULL */
            if (this->msg_queue_tail_ == msg)
                this->msg_queue_tail_ = NULL;

            /* put message to free list */
            msg->next = (struct mq_message *)this->msg_queue_free_;
            this->msg_queue_free_ = msg;
        }

        /* clean entry */
        this->entry_ = 0;

        /* enable interrupt */
        arch_interrupt_enable(level);

        Scheduler::process();

        return ERR_OK;
    }

    return -ERR_ERROR;
}
