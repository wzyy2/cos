/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef IPC_H
#define IPC_H

#include <cos/cosDef.h>
#include <cos/object.h>
#include <cos/thread.h>

/**
 * Class of Semaphore
 */
class IPC : public Object
{
public:
    IPC(object_class_type type, const char *name);
    ~IPC();

    static const uint8_t FLAG_FIFO = 0x00;  /**< FIFOed IPC. @ref IPC. */
    static const uint8_t FLAG_PRIO = 0x01;  /**< PRIOed IPC. @ref IPC. */
    static const uint8_t CMD_UNKNOWN = 0x00;    /**< unknown IPC command */
    static const uint8_t CMD_RESET = 0x01;  /**< reset IPC object */
    static const int8_t WAITING_FOREVER = -1;  /**< Block forever until get resource. */
    static const int8_t WAITING_NO = 0;     /**< Non-block. */

    err_t list_suspend(Thread *thread);
    err_t list_resume();
    err_t list_resume_all();

protected:

    coslib::List<Thread *> suspend_list_;  /**< the suspend thread list */
};

#endif // IPC_H

