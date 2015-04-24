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

private:
    uint32_t  set_;  /**< event set */
};

#endif // EVENT

