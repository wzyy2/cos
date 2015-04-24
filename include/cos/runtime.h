/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef _RUNTIME_H
#define _RUNTIME_H

#include <cos/cosDef.h>


class Runtime
{
public:
    /**< Flag show that if cpp runtime had been down. */
    static bool down_flag;

    static void boot_strap();
    static void exit();

private:
    Runtime();
    ~Runtime();
};

#endif
