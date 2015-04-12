#ifndef IDLE
#define IDLE

#include <cos/cosDef.h>
#include <cos/thread.h>

class Idle
{
public:

    static void init(void);
    static void entry(void *parameter);
    static void excute(void);

private:

    Idle();
    ~Idle();
};

#endif // IDLE

