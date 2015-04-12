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
