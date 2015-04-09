#ifndef _COSDEBUG_H
#define _COSDEBUG_H

#include <cos/cosConf.h>

/* Using this macro to control all kernel debug features. */
#ifdef COS_DEBUG

/* Turn on some of these (set to non-zero) to debug kernel */
#ifndef COS_EBUG_COMMON
#define COS_DEBUG_COMMON                0
#endif

#ifndef COS_DEBUG_MEM
#define COS_DEBUG_MEM                   0
#endif

#ifndef COS_DEBUG_MEMHEAP
#define COS_DEBUG_MEMHEAP               0
#endif

#ifndef COS_DEBUG_MODULE
#define COS_DEBUG_MODULE                0
#endif

#ifndef COS_DEBUG_SCHEDULER
#define COS_DEBUG_SCHEDULER             0
#endif

#ifndef COS_DEBUG_SLAB
#define COS_DEBUG_SLAB                  0
#endif

#ifndef COS_DEBUG_THREAD
#define COS_DEBUG_THREAD                0
#endif

#ifndef COS_DEBUG_TIMER
#define COS_DEBUG_TIMER                 0
#endif

#ifndef COS_DEBUG_IRQ
#define COS_DEBUG_IRQ                   0
#endif

#ifndef COS_DEBUG_IPC
#define COS_DEBUG_IPC                   0
#endif

#ifndef COS_EBUG_INIT
#define COS_DEBUG_INIT                  0
#endif

/* Turn on this to enable context check */
#ifndef COS_DEBUG_CONTEXT_CHECK
#define COS_DEBUG_CONTEXT_CHECK         1
#endif

#define COS_DEBUG_LOG(type, message)                                           \
    do                                                                            \
{                                                                             \
    if (type)                                                                 \
    printk message;                                                   \
    }                                                                             \
    while (0)

#define COS_ASSERT(EX)                                                         \
    if (!(EX))                                                                    \
{                                                                             \
    volatile char dummy = 0;                                                  \
    printk("(%s) assert failed at %s:%d \n", #EX, __FUNCTION__, __LINE__);\
    while (dummy == 0);                                                       \
    }

/* Macro to check current context */
#define COS_DEBUG_NOT_IN_INTERRUPT                                             \
    do                                                                            \
{                                                                             \
    base_t level;                                                          \
    level = arch_interrupt_disable();                                        \
    if (arch_interrupt_get_nest() != 0)                                         \
{                                                                         \
    printk("Function[%s] shall not used in ISR\n", __FUNCTION__);     \
    COS_ASSERT(0)                                                          \
    }                                                                         \
    arch_interrupt_enable(level);                                            \
    }                                                                             \
    while (0)


#else /* COS_DEBUG */

#define COS_ASSERT(EX)
#define COS_DEBUG_LOG(type, message)
#define COS_DEBUG_NOT_IN_INTERRUPT


#endif /* COS_DEBUG */

#endif /* _COSDEBUG_H */
