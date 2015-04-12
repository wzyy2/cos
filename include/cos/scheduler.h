#ifndef SCHEDULER
#define SCHEDULER

#include <cos/cosDef.h>
#include <cos/thread.h>
#include <cos/idle.h>

class Scheduler
{
public:
    friend class Idle;

    static void init();
    static void process();
    static void start();
    static void insert_thread(Thread *thread);
    static void remove_thread(Thread *thread);

    static void insert_defunct_thread(Thread *thread);

    static void enter_critical();
    static void exit_critical();
    static uint16_t critical_level();

    static Thread *get_current_thread();

private:
    Scheduler();
    ~Scheduler();

    static Thread *current_thread_;
    static uint8_t current_priority_;

    static int16_t lock_nest_;

    static const uint8_t THREAD_PRIORITY_MAX = 255;

    static std::set<Thread *, Thread::Compare> thread_set_;  /**< the thread set */
    static std::list<Thread *> defunct_list_;  /**< the defunct thread list */

};

#endif // SCHEDULER

