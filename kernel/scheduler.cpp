#include "cos/scheduler.h"

#include <cos/cos.h>
#include <arch/arch.h>

int16_t Scheduler::lock_nest_ = 0;
Thread *Scheduler::current_thread_ = NULL;
uint8_t Scheduler::current_priority_ = THREAD_PRIORITY_MAX - 1;
std::set<Thread *, Thread::Compare> Scheduler::thread_set_;
std::list<Thread *> Scheduler::defunct_list_;


Thread *Scheduler::get_current_thread()
{
    return current_thread_;
}

void Scheduler::start()
{
    register Thread *to_thread;

    /* get switch to thread */
    to_thread = *(thread_set_.begin());

    current_thread_ = to_thread;

    /* switch to new thread */
    arch_context_switch_to((uint32_t)&to_thread->sp_);

    /* never come back */
};

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
        to_thread = *(thread_set_.begin());

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
                          interrupt_get_nest(), highest_ready_priority,
                          to_thread->name.c_str(), to_thread->sp_,
                          from_thread->name.c_str(), from_thread->sp_));

            if (interrupt_get_nest() == 0)
            {
                arch_context_switch((uint32_t)&from_thread->sp_,
                                     (uint32_t)&to_thread->sp_);
            }
            else
            {
                COS_DEBUG_LOG(COS_DEBUG_SCHEDULER, ("switch in interrupt\n"));
                arch_context_switch_interrupt((uint32_t)&from_thread->sp_,
                                               (uint32_t)&to_thread->sp_);
            }
        }
    }

    /* enable interrupt */
    arch_interrupt_enable(level);
};

/**
 * This function will insert a thread to system ready queue. The state of
 * thread will be set as READY and remove from suspend queue.
 *
 * @param thread the thread to be inserted
 * @note Please do not invoke this function in user application.
 */
void Scheduler::insert_thread(Thread *thread)
{
    register base_t temp;

    COS_ASSERT(thread != NULL);

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* change stat */
    thread->stat_ = Thread::THREAD_READY;

    /* insert thread to ready set */
    thread_set_.insert(thread);

    /* set priority mask */
    COS_DEBUG_LOG(COS_DEBUG_SCHEDULER, ("insert thread[%s], the priority: %d\n",
                                      thread->name_.c_str(), thread->current_priority_));


    //rt_thread_ready_priority_group |= thread->number_mask_;

    /* enable interrupt */
    arch_interrupt_enable(temp);
};

/**
 * This function will remove a thread from system ready queue.
 *
 * @param thread the thread to be removed
 *
 * @note Please do not invoke this function in user application.
 */
void Scheduler::remove_thread(Thread *thread)
{
    register base_t temp;

    COS_ASSERT(thread != NULL);

    /* disable interrupt */
    temp = arch_interrupt_disable();

    COS_DEBUG_LOG(RT_DEBUG_SCHEDULER, ("remove thread[%s], the priority: %d\n",
                                      thread->name_.c_str(),
                                      thread->current_priority_));

    /* remove thread from ready list */
    thread_set_.erase(thread);

    /* enable interrupt */
    arch_interrupt_enable(temp);
};

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
};

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

};

/**
 * Get the scheduler lock level
 *
 * @return the level of the scheduler lock. 0 means unlocked.
 */
uint16_t Scheduler::critical_level()
{
    return lock_nest_;
};

/**
 * This function will insert a thread to system delete list.
 */
void Scheduler::insert_defunct_thread(Thread *thread)
{
    defunct_list_.push_back(thread);
}