/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "cos/ipc/ipc.h"
#include <cos/cos.h>


IPC::IPC(object_class_type type, const char *name)
    :Object(type, name)
{

}

IPC::~IPC()
{

}


/**
 * This function will suspend a thread to a specified list. IPC object or some
 * double-queue object (mailbox etc.) contains this kind of list.
 *
 * @param thread the thread object to be suspended
 *
 * @return the operation status, ERR_EOK on successful
 */
err_t IPC::list_suspend(Thread *thread){
    thread->suspend();

    switch (thread->flag_)
    {
    case FLAG_FIFO:
        suspend_list_.push_back(thread->list_node_);
        break;

    case FLAG_PRIO:
        suspend_list_.push_back(thread->list_node_);
        break;
    }

    return ERR_OK;
};

/**
 * This function will resume the first thread in the list of a IPC object:
 * - remove the thread from suspend queue of IPC object
 * - put the thread into system ready queue
 *
 * @param list the thread list
 *
 * @return the operation status, ERR_OK on successful
 */
err_t IPC::list_resume(){
    Thread *thread = suspend_list_.front();

    if(thread == NULL)
        return ERR_ERROR;

    COS_DEBUG_LOG(COS_DEBUG_IPC, ("resume thread:%s\n", thread->name()));
    /* resume it */
    suspend_list_.erase(thread->list_node_);
    thread->resume();

    return ERR_OK;
};

/**
 * This function will resume all suspended threads in a list, including
 * suspend list of IPC object and private list of mailbox etc.
 *
 * @param list of the threads to resume
 *
 * @return the operation status, ERR_OK on successful
 */
err_t IPC::list_resume_all(){
    Thread *thread;
    register ubase_t temp;

    /* wakeup all suspend threads */
    while (!suspend_list_.empty())
    {
        /* disable interrupt */
        temp = arch_interrupt_disable();

        /* get next suspend thread */
        thread = suspend_list_.front();
        /* set error code to ERR_ERROR */
        thread->error_ = -ERR_ERROR;

        thread->resume();

        /* enable interrupt */
        arch_interrupt_enable(temp);
    }

    return ERR_OK;
};
