#ifndef MAILBOX
#define MAILBOX

#include <cos/cosDef.h>
#include <cos/ipc/ipc.h>

class MailBox : public IPC
{
public:
    MailBox(const char *name,
            void        *msgpool,
            size_t    size,
            uint8_t  flag);
    MailBox(const char *name,
            size_t    size,
            uint8_t  flag);
    ~MailBox();


    err_t resume_all();
    err_t send_wait(uint32_t  value,
                             int32_t   timeout);
    err_t send(uint32_t value);
    err_t recv(uint32_t *value, int32_t timeout);
    err_t control(uint8_t cmd, void *arg);

private:
    uint32_t    *msg_pool_;                      /**< start address of message buffer */
    uint16_t    size_;                          /**< size of message pool */
    uint16_t    entry_;                         /**< index of messages in msg_pool */
    uint16_t    in_offset_;                     /**< input offset of the message buffer */
    uint16_t    out_offset_;                    /**< output offset of the message buffer */

    bool delete_pool_flag_;

    coslib::List<Thread *>  suspend_sender_thread_;         /**< sender thread suspended on this mailbox */

};

#endif // MAILBOX

