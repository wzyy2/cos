/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef THREAD
#define THREAD

#include <cos/cosDef.h>
#include <cos/clock.h>
#include <cos/kservice.h>
#include <cos/object.h>
#include <cos/timer.h>

/**
 * Class of Thread
 */
class Thread : public Object
{
public:
    friend class Scheduler;
    friend class IPC;
    friend class Semaphore;
    friend class Mutex;


    friend void tick_increase(void);
    friend err_t get_errno(void);
    friend void set_errno(err_t error);
    friend int *cos_errno(void);


    Thread(const char       *name,
           void (*entry)(void *parameter),
           void             *parameter,
           void             *stack_start,
           base_t       stack_size,
           uint8_t        priority,
           tick_t       tick);

    Thread(const char       *name,
           void (*entry)(void *parameter),
           void             *parameter,
           base_t       stack_size,
           uint8_t        priority,
           tick_t       tick);

    ~Thread();

    static err_t yield(void);
    static void exit(void);
    static void timeout(void *parameter);
    static err_t sleep(tick_t tick);
    static err_t delay(tick_t tick);
    static Thread *find(const char *name);

    uint8_t get_stat(void);
    err_t detach(bool delete_later);
    err_t startup();
    err_t control(uint8_t cmd, void *arg);
    err_t suspend();
    err_t resume();


    /**
     * thread state definitions
     */
    static const uint8_t THREAD_INIT = 0x00;                /**< Initialized status */
    static const uint8_t THREAD_READY = 0x01;                /**< Ready status */
    static const uint8_t THREAD_SUSPEND = 0x02;                /**< Suspend status */
    static const uint8_t THREAD_RUNNING = 0x03;                /**< Running status */
    static const uint8_t THREAD_BLOCK = THREAD_SUSPEND;   /**< Blocked status */
    static const uint8_t THREAD_CLOSE = 0x04;                /**< Closed status */

    /**
     * thread control command definitions
     */
    static const uint8_t CTRL_STARTUP = 0x00;               /**< Startup thread. */
    static const uint8_t CTRL_CLOSE = 0x01;                /**< Close thread. */
    static const uint8_t CTRL_CHANGE_PRIORITY = 0x02;                /**< Change thread priority. */
    static const uint8_t CTRL_INFO = 0x03;                /**< Get thread information. */

    /* error code */
    err_t    error_;                                  /**< error code */
    uint8_t  stat_;                                   /**< thread stat */
    Timer *thread_timer_;                       /**< built-in thread timer */

    coslib::List<Thread *>::Node  *list_node_;

    /* thread event */
    uint32_t event_set_;
    uint8_t  event_info_;

private:
    static std::set<Thread *> thread_set_;  /**< the thread set */

    /* stack point and entry */
    void       *sp_;                                     /**< stack point */
    void       *entry_;                                  /**< entry */
    void       *parameter_;                              /**< parameter */
    void       *stack_addr_;                             /**< stack address */
    ubase_t stack_size_;                             /**< stack size */

    /* priority */
    uint8_t  current_priority_;                       /**< current priority */
    uint8_t  init_priority_;                          /**< initialized priority */

    tick_t  init_tick_;                              /**< thread's initialized tick */
    tick_t  remaining_tick_;                         /**< remaining tick */

    uint32_t user_data_;                              /**< private user data beyond this thread */

    bool delete_stack_flag_ = false;

    coslib::RBTree<Thread *>::Node  *node_;
};



#endif // THREAD

