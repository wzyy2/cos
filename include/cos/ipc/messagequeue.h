#ifndef MESSAGEQUEUE
#define MESSAGEQUEUE

#include <cos/cosDef.h>
#include <cos/ipc/ipc.h>

class MessageQueue : public IPC
{
public:
    MessageQueue(const char *name, uint8_t  flag);
    ~MessageQueue();

private:

};

#endif // MESSAGEQUEUE

