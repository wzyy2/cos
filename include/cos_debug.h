#ifndef __COSDEBUG_H__
#define __COSDEBUG_H__

#include <cos_conf.h>

/* Using this macro to control all kernel debug features. */
#ifdef COS_DEBUG

#define COS_DEBUG_LOG(type, message)                                           \
do                                                                            \
{                                                                             \
    if (type)                                                                 \
        kprintf message;                                                   \
}                                                                             \
while (0)

#define COS_ASSERT(EX)                                                         \
if (!(EX))                                                                    \
{                                                                             \
    volatile char dummy = 0;                                                  \
    kprintf("(%s) assert failed at %s:%d \n", #EX, __FUNCTION__, __LINE__);\
    while (dummy == 0);                                                       \
}

#else /* COS_DEBUG */

#define COS_ASSERT(EX)
#define COS_DEBUG_LOG(type, message)
#define COS_DEBUG_NOT_IN_INTERRUPT
#define COS_DEBUG_IN_THREAD_CONTEXT

#endif /* COS_DEBUG */

#endif /* __COSDEBUG_H__ */