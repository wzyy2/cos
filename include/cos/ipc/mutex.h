/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef MUTEX
#define MUTEX

#include <cos/cosDef.h>
#include <cos/ipc/ipc.h>

class Mutex : public IPC
{
public:
    Mutex(const char *name,
          uint8_t  flag);
    ~Mutex();

    err_t take(int32_t time);
    err_t release();
    err_t control(uint8_t cmd, void *arg);


private:
    uint16_t          value_;                         /**< value of mutex */

    uint8_t           original_priority_;             /**< priority of last thread hold the mutex */
    uint8_t           hold_;                          /**< numbers of thread hold the mutex */

    Thread    *owner_;                         /**< current owner of mutex */

};


#endif // MUTEX

