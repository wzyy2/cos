/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef IDLE
#define IDLE

#include <cos/cosDef.h>
#include <cos/thread.h>

class Idle : Thread
{
public:
    static void init(void);
    static void entry(void *parameter);
    static void excute(void);

private:

    static uint8_t thread_stack[1024 * 4];

    Idle();
    ~Idle();
};

#endif // IDLE

