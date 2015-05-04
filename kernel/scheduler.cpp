/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "cos/scheduler.h"

#include <cos/cos.h>
#include <arch/arch.h>

int16_t Scheduler::lock_nest_ = 0;
Thread *Scheduler::current_thread_ = NULL;
uint8_t Scheduler::current_priority_ = THREAD_PRIORITY_MAX - 1;

coslib::RBTree<coslib::List<Thread *> *> Scheduler::priority_tree_;

coslib::List<Thread *> Scheduler::defunct_list_;

coslib::RBTree<coslib::List<Thread *> *>::Node *Scheduler::ready_table_[Scheduler::THREAD_PRIORITY_MAX];

Thread *Scheduler::get_current_thread()
{
    return current_thread_;
}

void Scheduler::init()
{
    for(int i = 0; i < THREAD_PRIORITY_MAX; i++) {
        ready_table_[i] = new coslib::RBTree<coslib::List<Thread *> *>::Node(i, new coslib::List<Thread *>());
    }
}

void Scheduler::start()
{
    register Thread *to_thread;

    /* get switch to thread */
    to_thread = next_thread();

    current_thread_ = to_thread;

    /* switch to new thread */
    arch_context_switch_to((ubase_t)&to_thread->sp_);

    /* never come back */
}

/**
 * This function will perform one schedule. It will select one thread
 * with the highest priority level, then switch to it.
 */
void Scheduler::process()
{
    base_t level;
    Thread *to_thread;
    Thread *from_thread;

    /* disable interrupt */
    level = arch_interrupt_disable();

    /* check the scheduler is enabled or not */
    if (lock_nest_ == 0)
    {
        /* get switch to thread */
        to_thread = next_thread();

        /* if the destination thread is not the same as current thread */
        if (to_thread != current_thread_)
        {
            //current_priority_ = (uint8_t)highest_ready_priority;
            from_thread = current_thread_;
            current_thread_ = to_thread;

            /* switch to new thread */
            COS_DEBUG_LOG(COS_DEBUG_SCHEDULER,
                         ("[%d]switch to priority#%d "
                          "thread:%s(sp:0x%p), "
                          "from thread:%s(sp: 0x%p)\n",
                          interrupt_get_nest(), to_thread->current_priority_,
                          to_thread->name(), to_thread->sp_,
                          from_thread->name(), from_thread->sp_));

            if (interrupt_get_nest() == 0)
            {
                arch_context_switch((ubase_t)&from_thread->sp_,
                                     (ubase_t)&to_thread->sp_);

            }
            else
            {
                COS_DEBUG_LOG(COS_DEBUG_SCHEDULER, ("switch in interrupt\n"));
                arch_context_switch_interrupt((ubase_t)&from_thread->sp_,
                                               (ubase_t)&to_thread->sp_);
            }
        }
    }

    /* enable interrupt */
    arch_interrupt_enable(level);
}

/**
 * This function will insert a thread to system ready queue. The state of
 * thread will be set as READY and remove from suspend queue.
 *
 * @param thread the thread to be inserted
 * @note Please do not invoke this function in user application.
 */
void Scheduler::insert_thread(Thread *thread)
{
    base_t temp;

    COS_ASSERT(thread != NULL);

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* change stat */
    thread->stat_ = Thread::THREAD_READY;

    auto node = ready_table_[thread->current_priority_];
    auto list = node->getObj();

    if(list->empty()) {
        /* insert node to priority tree */
        priority_tree_.insert(ready_table_[thread->current_priority_]);
    }

    list->push_back(thread->list_node_);

    /* set priority mask */
    COS_DEBUG_LOG(COS_DEBUG_SCHEDULER, ("insert thread[%s], the priority: %d\n",
                                      thread->name_, thread->current_priority_));


    /* enable interrupt */
    arch_interrupt_enable(temp);
}

/**
 * This function will remove a thread from system ready queue.
 *
 * @param thread the thread to be removed
 *
 * @note Please do not invoke this function in user application.
 */
void Scheduler::remove_thread(Thread *thread)
{
    base_t temp;

    COS_ASSERT(thread != NULL);

    /* disable interrupt */
    temp = arch_interrupt_disable();

    COS_DEBUG_LOG(COS_DEBUG_SCHEDULER, ("remove thread[%s], the priority: %d\n",
                                      thread->name_,
                                      thread->current_priority_));

    auto node = ready_table_[thread->current_priority_];
    auto list = node->getObj();

    list->erase(thread->list_node_);

    if(list->empty()) {
        /* remove node from priority tree */
        priority_tree_.remove(ready_table_[thread->current_priority_]);
    }

    /* enable interrupt */
    arch_interrupt_enable(temp);
}

/**
 * This function will return next ready thread.
 */
Thread *Scheduler::next_thread()
{
    Thread *to_thread = priority_tree_.min()->front();

    return to_thread;
}

/**
 * This function will lock the thread scheduler.
 */
void Scheduler::enter_critical()
{
    register base_t level;

    /* disable interrupt */
    level = arch_interrupt_disable();

    /*
     * the maximal number of nest is UINT16_MAX, which is big
     * enough and does not check here
     */
    lock_nest_++;

    /* enable interrupt */
    arch_interrupt_enable(level);
}

/**
 * This function will unlock the thread scheduler.
 */
void Scheduler::exit_critical()
{
    register base_t level;

    /* disable interrupt */
    level = arch_interrupt_disable();

    lock_nest_--;

    if (lock_nest_ <= 0)
    {
        lock_nest_ = 0;
        /* enable interrupt */
        arch_interrupt_enable(level);

        Scheduler::process();
    }
    else
    {
        /* enable interrupt */
        arch_interrupt_enable(level);
    }

}

/**
 * Get the scheduler lock level
 *
 * @return the level of the scheduler lock. 0 means unlocked.
 */
uint16_t Scheduler::critical_level()
{
    return lock_nest_;
}

/**
 * This function will insert a thread to system delete list.
 */
void Scheduler::insert_defunct_thread(Thread *thread)
{
    defunct_list_.push_back(thread->list_node_);
}

/**
 * This function will insert a thread to system delete list.
 */
void Scheduler::remove_defunct_thread(Thread *thread)
{
    defunct_list_.erase(thread->list_node_);
}

/**
 * This function will be called in systick.
 */
void Scheduler::inclock(tick_t tick)
{
    Thread *thread;
    base_t level;

    thread = current_thread_;

    if(thread != NULL) {
        -- thread->remaining_tick_;
        if (thread->remaining_tick_ == 0)
        {

            /* else change to initialized tick */
            thread->remaining_tick_ = thread->init_tick_;

            /* disable interrupt */
            level = arch_interrupt_disable();

            /* if the thread stat is READY and on ready queue set */
            if (thread->stat_ == Thread::THREAD_READY)
            {
                /* put thread to end of ready table */
                auto node = ready_table_[thread->current_priority_];
                auto list = node->getObj();
                list->erase(thread->list_node_);
                list->push_back(thread->list_node_);

                /* enable interrupt */
                arch_interrupt_enable(level);

                Scheduler::process();

                return;
            }

            /* enable interrupt */
            arch_interrupt_enable(level);
        }
    }
}
