#include "cos/idle.h"

#include <cos/cos.h>

uint8_t Idle::thread_stack[1024];

Idle::Idle():Thread("idle", entry, NULL, &Idle::thread_stack[0] \
                    , sizeof(thread_stack), Scheduler::THREAD_PRIORITY_MAX - 1, 32)
{

}

Idle::~Idle()
{

}

/**
 * This function will perform system background job when system idle.
 */
void Idle::excute(void)
{
    /* Loop until there is no dead thread. So one call to rt_thread_idle_excute
     * will do all the cleanups. */
    while (Scheduler::defunct_list_.size())
    {
        base_t lock;
        Thread * thread;

        COS_DEBUG_NOT_IN_INTERRUPT;

        /* disable interrupt */
        lock = arch_interrupt_disable();

        /* re-check whether list is empty */
        if (Scheduler::defunct_list_.size())
        {
            /* get defunct thread */
            thread = Scheduler::defunct_list_.front();

            /* remove defunct thread */
            Scheduler::defunct_list_.pop_front();

            delete thread;
        }
        else
        {
            /* enable interrupt */
            arch_interrupt_enable(lock);

            /* may the defunct thread list is removed by others, just return */
            return;
        }

        /* enable interrupt */
        arch_interrupt_enable(lock);

    }
}

void Idle::entry(void *parameter)
{
    while (1)
    {
        excute();
    }
}

/**
 *
 * This function will initialize idle thread, then start it.
 *
 * @note this function must be invoked when system init.
 */
void Idle::init(void)
{
    /* initialize thread */
    Idle *idle = new Idle();

    /* startup */
    idle->startup();
}
