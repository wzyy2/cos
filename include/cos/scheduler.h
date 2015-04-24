/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef SCHEDULER
#define SCHEDULER

#include <cos/cosDef.h>
#include <cos/thread.h>
#include <cos/idle.h>

class Scheduler
{
public:
    friend class Idle;
    friend class Thread;

    static void init();
    static void process();
    static void start();
    static void insert_thread(Thread *thread);
    static void remove_thread(Thread *thread);

    static void insert_defunct_thread(Thread *thread);
    static void remove_defunct_thread(Thread *thread);

    static void enter_critical();
    static void exit_critical();
    static uint16_t critical_level();

    static Thread *get_current_thread();

    static const uint8_t THREAD_PRIORITY_MAX = 255;


    static coslib::List<Thread *> defunct_list_;  /**< the defunct thread list */


private:
    Scheduler();
    ~Scheduler();

    static Thread *current_thread_;
    static uint8_t current_priority_;

    static int16_t lock_nest_;

    static coslib::RBTree<Thread *> thread_tree_;  /**< the thread tree */

};

#endif // SCHEDULER

