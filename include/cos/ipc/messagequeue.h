#ifndef MESSAGEQUEUE
#define MESSAGEQUEUE

#include <cos/cosDef.h>
#include <cos/ipc/ipc.h>

class MessageQueue : public IPC
{
public:
    MessageQueue(const char *name, size_t   msg_size,
                 size_t   pool_size, uint8_t  flag);
    ~MessageQueue();

    err_t send(void *buffer, size_t size);
    err_t urgent(void *buffer, size_t size);
    err_t recv(void      *buffer,
                             size_t  size,
                             int32_t timeout);
    err_t control(uint8_t cmd, void *arg);

private:
    struct mq_message
    {
        struct mq_message *next;
    };

    void    *msg_pool_;                      /**< start address of message queue */

    uint16_t msg_size_;                      /**< message size of each message */
    uint16_t max_msgs_;                      /**< max number of messages */

    uint16_t entry_;                         /**< index of messages in the queue */

    void    *msg_queue_head_;                /**< list head */
    void    *msg_queue_tail_;                /**< list tail */
    void    *msg_queue_free_;                /**< pointer indicated the free node of queue */
};

#endif // MESSAGEQUEUE

