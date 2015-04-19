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

    static uint8_t thread_stack[1024];

    Idle();
    ~Idle();
};

#endif // IDLE

