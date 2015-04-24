/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "cos/ipc/event.h"

/**
 * This function will initialize an event and put it under control of resource
 * management.
 *
 * @param name the name of event
 * @param flag the flag of event
 */
Event::Event(const char *name,
             uint8_t  flag):IPC(Object::Object_Class_Event, name)
{
    flag_ = flag;
    set_ = 0;
}

Event::~Event()
{

}

