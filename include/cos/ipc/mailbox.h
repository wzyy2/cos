#ifndef MAILBOX
#define MAILBOX

#include <cos/cosDef.h>
#include <cos/ipc/ipc.h>

class MailBox : public IPC
{
public:
    MailBox(const char *name, uint8_t  flag);
    ~MailBox();


private:

};

#endif // MAILBOX

