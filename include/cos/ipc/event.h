/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef EVENT
#define EVENT

#include <cos/cosDef.h>
#include <cos/ipc/ipc.h>

class Event : public IPC
{
public:
    Event(const char *name,
          uint8_t  flag);
    ~Event();

    err_t control(uint8_t cmd, void *arg);
    err_t recv(uint32_t  set,
                           uint8_t   option,
                           int32_t   timeout,
                           uint32_t *recved);
    err_t send(uint32_t set);


    static const uint8_t FLAG_AND       =      0x01;   /**< logic and */
    static const uint8_t FLAG_OR       =        0x02;     /**< logic or */
    static const uint8_t FLAG_CLEAR    =        0x04;       /**< clear flag */

private:
    uint32_t  set_;  /**< event set */
};

#endif // EVENT

