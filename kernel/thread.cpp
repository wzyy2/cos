#include "cos/thread.h"

#include <cos/cos.h>
#include <arch/arch.h>

/**
 * This function will initialize a thread, normally it's used to initialize a
 * static thread object.
 *
 * @param name the name of thread, which shall be unique
 * @param entry the entry function of thread
 * @param parameter the parameter of thread enter function
 * @param stack_start the start address of thread stack
 * @param stack_size the size of thread stack
 * @param priority the priority of thread
 * @param tick the time slice if there are same priority thread
 *
 */
Thread::Thread(const char       *name,
               void (*entry)(void *parameter),
               void             *parameter,
               void             *stack_start,
               base_t       stack_size,
               uint8_t        priority,
               tick_t       tick):Object(Object_Class_Thread, name)
{
    COS_ASSERT(stack_start != NULL);

    entry_ = (void *)entry;
    parameter_ = parameter;

    /* stack init */
    stack_addr_ = stack_start;
    stack_size_ = stack_size;

    /* init thread stack */
    memset(stack_addr_, '#', stack_size_);
    sp_ = (void *)arch_stack_init(entry_, parameter_,
                                  (uint8_t *)((char *)stack_addr_ + stack_size_ - 4),
                                  (void *)exit);

    /* priority init */
    init_priority_    = priority;
    current_priority_ = priority;

    /* tick init */
    init_tick_      = tick;
    remaining_tick_ = tick;

    /* error and flags */
    error_ = ERR_OK;
    stat_  = THREAD_INIT;

    /* initialize cleanup function and user data */
    user_data_ = 0;

    /* init thread timer */
    thread_timer_ = new Timer(name, Thread::timeout, this, 0, Timer::FLAG_ONE_SHOT);

    node_ = new coslib::RBTree<Thread>::RBTreeNode (current_priority_, this, NULL);
}

Thread::~Thread()
{
    detach(false);
    delete thread_timer_;
}

void Thread::exit(void)
{
    Thread *thread;
    register base_t level;

    /* get current thread */
    thread = Scheduler::get_current_thread();

    /* disable interrupt */
    level = arch_interrupt_disable();

    /* remove from schedule */
    Scheduler::remove_thread(thread);

    thread->detach(true);

    /* enable interrupt */
    arch_interrupt_enable(level);

    /* switch to next task */
    Scheduler::process();
}

/**
 * This function will start a thread and put it to system ready queue
 *
 * @return the operation status, ERR_OK on OK, -ERR_ERROR on error
 */
err_t Thread::startup()
{
    /* thread check */
    COS_ASSERT(stat_ == THREAD_INIT);

    /* set current priority to init priority */
    current_priority_ = init_priority_;

    /* calculate priority attribute */

    COS_DEBUG_LOG(COS_DEBUG_THREAD, ("startup a thread:%s with priority:%d\n",
                                     name_.c_str(), init_priority_));
    /* change thread stat */
    stat_ = THREAD_SUSPEND;
    /* then resume it */
    resume();
    if (Scheduler::get_current_thread() != NULL)
    {
        /* do a scheduling */
        Scheduler::process();
    }

    return ERR_OK;
}

/**
 * This function will detach a thread. The thread object will be removed from
 * thread queue and detached/deleted from system object management.
 *
 * @param delete_later if the thread need be deleted by idle
 *
 * @return the operation status, ERR_OK on OK, -ERR_ERROR on error
 */
err_t Thread::detach(bool delete_later)
{
    base_t lock;

    /* remove from schedule */
    Scheduler::remove_thread(this);

    /* release thread timer */
    thread_timer_->detach();

    /* change stat */
    stat_ = THREAD_CLOSE;

    /* detach object */
    Object::detach();

    if(delete_later)
    {
        /* disable interrupt */
        lock = arch_interrupt_disable();

        /* insert to defunct thread list */
        Scheduler::insert_defunct_thread(this);

        /* enable interrupt */
        arch_interrupt_enable(lock);
    }

    return ERR_OK;
}

/**
 * This function will let current thread yield processor, and scheduler will
 * choose a highest thread to run. After yield processor, the current thread
 * is still in READY state.
 *
 * @return ERR_OK
 *
 * @note This function  will be called in interrupt status.
 */
err_t Thread::yield(void)
{
    register base_t level;
    Thread *thread;

    /* disable interrupt */
    level = arch_interrupt_disable();

    /* set to current thread */
    thread = Scheduler::get_current_thread();

    /* if the thread stat is READY and on ready queue set */
    if (thread->stat_ == THREAD_READY)
    {
        /* remove thread from thread set */
        Scheduler::remove_thread(thread);
        /* put thread to end of ready queue */
        Scheduler::insert_thread(thread);

        /* enable interrupt */
        arch_interrupt_enable(level);

        Scheduler::process();

        return ERR_OK;
    }

    /* enable interrupt */
    arch_interrupt_enable(level);

    return ERR_OK;
}

/**
 * This function will let current thread sleep for some ticks.
 *
 * @param tick the sleep ticks
 *
 * @return ERR_OK
 */
err_t Thread::sleep(tick_t tick)
{
    register base_t temp;
    struct Thread *thread;

    /* disable interrupt */
    temp = arch_interrupt_disable();
    /* set to current thread */
    thread = Scheduler::get_current_thread();
    COS_ASSERT(thread != NULL);

    /* suspend thread */
    thread->suspend();

    /* reset the timeout of thread timer and start it */
    thread->thread_timer_->control(Timer::CTRL_SET_TIME, &tick);
    thread->thread_timer_->start();

    /* enable interrupt */
    arch_interrupt_enable(temp);

    Scheduler::process();

    /* clear error number of this thread to ERR_OK */
    if (thread->error_ == -ERR_TIMEOUT)
        thread->error_ = ERR_OK;

    return ERR_OK;
}

/**
 * This function will let current thread delay for some ticks.
 *
 * @param tick the delay ticks
 *
 * @return ERR_OK
 */
err_t Thread::delay(tick_t tick)
{
    return sleep(tick);
}


/**
 * This function will control thread behaviors according to control command.
 *
 * @param cmd the control command, which includes
 *  CTRL_CHANGE_PRIORITY for changing priority level of thread;
 *  CTRL_STARTUP for starting a thread;
 * @param arg the argument of control command
 *
 * @return ERR_OK
 */
err_t Thread::control(uint8_t cmd, void *arg)
{
    register base_t temp;

    switch (cmd)
    {
    case Thread::CTRL_CHANGE_PRIORITY:
        /* disable interrupt */
        temp = arch_interrupt_disable();

        /* for ready thread, change queue */
        if (stat_ == Thread::THREAD_READY)
        {
            /* remove thread from schedule queue first */
            Scheduler::remove_thread(this);

            /* change thread priority */
            current_priority_ = *(uint8_t *)arg;

            /* recalculate priority attribute */
            //thread->number_mask = 1 << thread->current_priority;

            /* insert thread to schedule queue again */
            Scheduler::insert_thread(this);
        }
        else
        {
            current_priority_ = *(uint8_t *)arg;

            /* recalculate priority attribute */

            //thread->number_mask = 1 << thread->current_priority;

        }
        /* enable interrupt */
        arch_interrupt_enable(temp);
        break;

    case Thread::CTRL_STARTUP:
        return startup();
    default:
        break;
    }

    return ERR_OK;
}


/**
 * This function will suspend the specified thread.
 *
 * @return the operation status, ERR_OK on OK, -ERR_ERROR on error
 */
err_t Thread::suspend()
{
    register base_t temp;

    COS_DEBUG_LOG(COS_DEBUG_THREAD, ("thread suspend:  %s\n", name_.c_str()));

    if (stat_ != THREAD_READY)
    {
        COS_DEBUG_LOG(COS_DEBUG_THREAD, ("thread suspend: thread disorder, %d\n",
                                         stat_));

        return -ERR_ERROR;
    }

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* change thread stat */
    stat_ = THREAD_SUSPEND;
    Scheduler::remove_thread(this);

    /* stop thread timer anyway */
    thread_timer_->stop();

    /* enable interrupt */
    arch_interrupt_enable(temp);

    return ERR_OK;
}


/**
 * This function will resume a thread and put it to system ready queue.
 *
 * @return the operation status, ERR_OK on OK, -ERR_ERROR on error
 */
err_t Thread::resume()
{
    register base_t temp;

    COS_DEBUG_LOG(COS_DEBUG_THREAD, ("thread resume:  %s\n", name_.c_str()));

    if (stat_ != THREAD_SUSPEND)
    {
        COS_DEBUG_LOG(COS_DEBUG_THREAD, ("thread resume: thread disorder, %d\n",
                                         stat_));

        return -ERR_ERROR;
    }

    /* disable interrupt */
    temp = arch_interrupt_disable();

    /* remove from suspend list */
    Scheduler::remove_thread(this);

    thread_timer_->stop();

    /* enable interrupt */
    arch_interrupt_enable(temp);

    /* insert to schedule ready list */
    Scheduler::insert_thread(this);

    return ERR_OK;
}

/**
 * This function is the timeout function for thread, normally which is invoked
 * when thread is timeout to wait some resource.
 *
 * @param parameter the parameter of thread timeout function
 */
void Thread::timeout(void *parameter)
{
    Thread *thread;

    thread = (Thread *)parameter;

    /* thread check */
    COS_ASSERT(thread != NULL);
    COS_ASSERT(thread->stat_ == THREAD_SUSPEND);

    /* set error number */
    thread->error_ = -ERR_TIMEOUT;

    /* remove from suspend list */
    //rt_list_remove(&(thread->tlist));
    Scheduler::remove_thread(thread);

    /* insert to schedule ready list */
    Scheduler::insert_thread(thread);

    /* do schedule */
    Scheduler::process();


}

/**
 * This function will find the specified thread.
 *
 * @param name the name of thread finding
 *
 * @return the found thread
 *
 * @note please don't invoke this function in interrupt status.
 */
Thread *Thread::find(const char *name)
{
    Object *obj = Object::find(name, Object::Object_Class_Device);
    return (Thread *) obj;
}

uint8_t Thread::get_stat(void)
{
    return stat_;
}
