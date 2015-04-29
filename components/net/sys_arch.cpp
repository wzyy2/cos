/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <cos/cos.h>

#include "lwip/sys.h"
#include "lwip/opt.h"
#include "lwip/stats.h"
#include "lwip/err.h"
#include "arch/sys_arch.h"
#include "lwip/debug.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"

#include "lwip/sio.h"
#include <lwip/init.h>

#include <string.h>


/*
 * Initialize the network interface device
 *
 * @return the operation status, ERR_OK on OK, ERR_IF on error
 */
static err_t netif_device_init(struct netif *netif)
{
//    EthDevice *ethif;

//    ethif = (EthDevice *)netif->state;
//    if (ethif != NULL)
//    {
//        if (ethif->init() != ERR_OK)
//        {
//            return ERR_IF;
//        }

//        /* copy device flags to netif flags */
//        netif->flags = ethif->getflag();

//        return ERR_OK;
//    }

    return ERR_IF;
}
/*
 * Initialize the ethernetif layer and set network interface device up
 */
static void tcpip_init_done_callback(void *arg)
{
    Device *device;
    //struct eth_device *ethif;
    struct ip_addr ipaddr, netmask, gw;

    LWIP_ASSERT("invalid arg.\n",arg);

    IP4_ADDR(&gw, 0,0,0,0);
    IP4_ADDR(&ipaddr, 0,0,0,0);
    IP4_ADDR(&netmask, 0,0,0,0);

    /* enter critical */
    Scheduler::enter_critical();

//    std::map<std::string, Object *> map_device = Object::getinfomation(Object::Object_Class_Device);
//    auto ptr = map_device.begin();
//    /* for each network interfaces */
//    for(; ptr != map_device.end(); ptr++) {

//        if (device->get_type() == Device::Device_Class_NetIf)
//        {
//            ethif = (struct eth_device *)device;

//            /* leave critical */
//            Scheduler::exit_critical();

//            netif_add(ethif->netif, &ipaddr, &netmask, &gw,
//                      ethif, netif_device_init, tcpip_input);

//            if (netif_default == NULL)
//                netif_set_default(ethif->netif);

//#if LWIP_DHCP
//            if (ethif->flags & NETIF_FLAG_DHCP)
//            {
//                /* if this interface uses DHCP, start the DHCP client */
//                dhcp_start(ethif->netif);
//            }
//            else
//#endif
//            {
//                /* set interface up */
//                netif_set_up(ethif->netif);
//            }

//#if LWIP_NETIF_LINK_CALLBACK
//            netif_set_link_up(ethif->netif);
//#endif

//            /* enter critical */
//            Scheduler::enter_critical();
//        }
//    }

    /* leave critical */
    Scheduler::exit_critical();
    ((Semaphore *)arg)->release();
}

/**
 * LwIP system initialization
 */
int lwip_system_init(void)
{
    Semaphore *done_sem;

    /* set default netif to NULL */
    netif_default = NULL;

    done_sem = new Semaphore("done", 0, IPC::FLAG_FIFO);

    if (get_errno() != ERR_OK)
    {
        LWIP_ASSERT("Failed to create semaphore", 0);

        return -1;
    }

    tcpip_init(tcpip_init_done_callback, (void *)done_sem);

    /* waiting for initialization done */
    if (done_sem->take(IPC::WAITING_FOREVER) != ERR_OK)
    {
        delete done_sem;

        return -1;
    }
    delete done_sem;

    /* set default ip address */
#if !LWIP_DHCP
    if (netif_default != NULL)
    {
        struct ip_addr ipaddr, netmask, gw;

        IP4_ADDR(&ipaddr, RT_LWIP_IPADDR0, RT_LWIP_IPADDR1, RT_LWIP_IPADDR2, RT_LWIP_IPADDR3);
        IP4_ADDR(&gw, RT_LWIP_GWADDR0, RT_LWIP_GWADDR1, RT_LWIP_GWADDR2, RT_LWIP_GWADDR3);
        IP4_ADDR(&netmask, RT_LWIP_MSKADDR0, RT_LWIP_MSKADDR1, RT_LWIP_MSKADDR2, RT_LWIP_MSKADDR3);

        netifapi_netif_set_addr(netif_default, &ipaddr, &netmask, &gw);
    }
#endif
    printk("lwIP-%d.%d.%d initialized!\n", LWIP_VERSION_MAJOR, LWIP_VERSION_MINOR, LWIP_VERSION_REVISION);

    return 0;
}


void sys_init(void)
{
    /* nothing on porting */
}

void lwip_sys_init(void)
{
    lwip_system_init();
}
INIT_COMPONENT_EXPORT(lwip_sys_init);

/*
 * Create a new semaphore
 *
 * @return the operation status, ERR_OK on OK; others on error
 */
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    static unsigned short counter = 0;
    char tname[CONFIG_NAME_MAX];
    Semaphore *tmpsem;

    COS_DEBUG_NOT_IN_INTERRUPT;

    snprintf(tname, CONFIG_NAME_MAX, "%s%d", SYS_LWIP_SEM_NAME, counter);
    counter ++;

    tmpsem = new Semaphore(tname, count, IPC::FLAG_FIFO);
    if (tmpsem == NULL)
        return ERR_MEM;
    else
    {
        *sem = (void *)tmpsem;

        return ERR_OK;
    }
}

/*
 * Deallocate a semaphore
 */
void sys_sem_free(sys_sem_t *sem)
{
    COS_DEBUG_NOT_IN_INTERRUPT;
    delete ((Semaphore *)*sem);
}

/*
 * Signal a semaphore
 */
void sys_sem_signal(sys_sem_t *sem)
{
    ((Semaphore *)*sem)->release();
}

/*
 * Block the thread while waiting for the semaphore to be signaled
 *
 * @return If the timeout argument is non-zero, it will return the number of milliseconds
 *         spent waiting for the semaphore to be signaled; If the semaphore isn't signaled
 *         within the specified time, it will return SYS_ARCH_TIMEOUT; If the thread doesn't
 *         wait for the semaphore, it will return zero
 */
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    err_t ret;
    s32_t t;
    u32_t tick;

    COS_DEBUG_NOT_IN_INTERRUPT;

    /* get the begin tick */
    tick = tick_get();
    if (timeout == 0)
        t = IPC::WAITING_FOREVER;
    else
    {
        /* convert msecond to os tick */
        if (timeout < (1000/CONFIG_TICK_PER_SECOND))
            t = 1;
        else
            t = timeout / (1000/CONFIG_TICK_PER_SECOND);
    }

    ret = ((Semaphore *)*sem)->take(t);

    if (ret == -ERR_TIMEOUT)
        return SYS_ARCH_TIMEOUT;
    else
    {
        if (ret == ERR_OK)
            ret = 1;
    }

    /* get elapse msecond */
    tick = tick_get() - tick;

    /* convert tick to msecond */
    tick = tick * (1000 / CONFIG_TICK_PER_SECOND);
    if (tick == 0)
        tick = 1;

    return tick;
}

#ifndef sys_sem_valid
/** Check if a semaphore is valid/allocated:
 *  return 1 for valid, 0 for invalid
 */
int sys_sem_valid(sys_sem_t *sem)
{
    return (int)(*sem);
}
#endif

#ifndef sys_sem_set_invalid
/** Set a semaphore invalid so that sys_sem_valid returns 0
 */
void sys_sem_set_invalid(sys_sem_t *sem)
{
    *sem = NULL;
}
#endif

/* ====================== Mutex ====================== */

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex
 */
err_t sys_mutex_new(sys_mutex_t *mutex)
{
    static unsigned short counter = 0;
    char tname[CONFIG_NAME_MAX];
    Mutex *tmpmutex;

    COS_DEBUG_NOT_IN_INTERRUPT;

    snprintf(tname, CONFIG_NAME_MAX, "%s%d", SYS_LWIP_MUTEX_NAME, counter);
    counter ++;

    tmpmutex = new Mutex(tname, IPC::FLAG_FIFO);
    if (tmpmutex == NULL)
        return ERR_MEM;
    else
    {
        *mutex = (void *) tmpmutex;

        return ERR_OK;
    }
}

/** Lock a mutex
 * @param mutex the mutex to lock
 */
void sys_mutex_lock(sys_mutex_t *mutex)
{
    COS_DEBUG_NOT_IN_INTERRUPT;

    ((Mutex *)*mutex)->take(IPC::WAITING_FOREVER);

    return;
}

/** Unlock a mutex
 * @param mutex the mutex to unlock
 */
void sys_mutex_unlock(sys_mutex_t *mutex)
{
    ((Mutex *)*mutex)->release();
}

/** Delete a semaphore
 * @param mutex the mutex to delete
 */
void sys_mutex_free(sys_mutex_t *mutex)
{
    COS_DEBUG_NOT_IN_INTERRUPT;

    delete ((Mutex *)*mutex);
}

#ifndef sys_mutex_valid
/** Check if a mutex is valid/allocated:
 *  return 1 for valid, 0 for invalid
 */
int sys_mutex_valid(sys_mutex_t *mutex)
{
    return (int)(*mutex);
}
#endif

#ifndef sys_mutex_set_invalid
/** Set a mutex invalid so that sys_mutex_valid returns 0
 */
void sys_mutex_set_invalid(sys_mutex_t *mutex)
{
    *mutex = NULL;
}
#endif

/* ====================== Mailbox ====================== */

/*
 * Create an empty mailbox for maximum "size" elements
 *
 * @return the operation status, ERR_OK on OK; others on error
 */
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    static unsigned short counter = 0;
    char tname[CONFIG_NAME_MAX];
    sys_mbox_t tmpmbox;

    COS_DEBUG_NOT_IN_INTERRUPT;

    snprintf(tname, CONFIG_NAME_MAX, "%s%d", SYS_LWIP_MBOX_NAME, counter);
    counter ++;

    tmpmbox = new MailBox(tname, size, IPC::FLAG_FIFO);
    if (tmpmbox != NULL)
    {
        *mbox = tmpmbox;

        return ERR_OK;
    }

    return ERR_MEM;
}

/*
 * Deallocate a mailbox
 */
void sys_mbox_free(sys_mbox_t *mbox)
{
    COS_DEBUG_NOT_IN_INTERRUPT;

    delete ((MailBox *)*mbox);

    return;
}

/** Post a message to an cmbox - may not fail
 * -> blocks if full, only used from tasks not from ISR
 * @param mbox mbox to posts the message
 * @param msg message to post (ATTENTION: can be NULL)
 */
void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    COS_DEBUG_NOT_IN_INTERRUPT;

    ((MailBox *)*mbox)->send_wait((uint32_t)msg, IPC::WAITING_FOREVER);

    return;
}

/*
 * Try to post the "msg" to the mailbox
 *
 * @return return ERR_OK if the "msg" is posted, ERR_MEM if the mailbox is full
 */
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    if (((MailBox *)*mbox)->send((uint32_t)msg) == ERR_OK)
        return ERR_OK;

    return ERR_MEM;
}

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return time (in milliseconds) waited for a message, may be 0 if not waited
           or SYS_ARCH_TIMEOUT on timeout
 *         The returned time has to be accurate to prevent timer jitter!
 */
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    err_t ret;
    s32_t t;
    u32_t tick;

    COS_DEBUG_NOT_IN_INTERRUPT;

    /* get the begin tick */
    tick = tick_get();

    if(timeout == 0)
        t = IPC::WAITING_FOREVER;
    else
    {
        /* convirt msecond to os tick */
        if (timeout < (1000/CONFIG_TICK_PER_SECOND))
            t = 1;
        else
            t = timeout / (1000/CONFIG_TICK_PER_SECOND);
    }

    ret = ((MailBox *)*mbox)->recv((uint32_t *)msg, t);

    if(ret == -ERR_TIMEOUT)
        return SYS_ARCH_TIMEOUT;
    else
    {
        LWIP_ASSERT("rt_mb_recv returned with error!", ret == ERR_OK);
    }

    /* get elapse msecond */
    tick = tick_get() - tick;

    /* convert tick to msecond */
    tick = tick * (1000 / CONFIG_TICK_PER_SECOND);
    if (tick == 0)
        tick = 1;

    return tick;
}

/** Wait for a new message to arrive in the mbox
 * @param mbox mbox to get a message from
 * @param msg pointer where the message is stored
 * @param timeout maximum time (in milliseconds) to wait for a message
 * @return 0 (milliseconds) if a message has been received
 *         or SYS_MBOX_EMPTY if the mailbox is empty
 */
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    int ret;

    ret = ((MailBox *)*mbox)->recv((uint32_t *)msg, 0);

    if(ret == -ERR_TIMEOUT)
        return SYS_ARCH_TIMEOUT;
    else
    {
        if (ret == ERR_OK)
            ret = 1;
    }

    return ret;
}

#ifndef sys_mbox_valid
/** Check if an mbox is valid/allocated:
 *  return 1 for valid, 0 for invalid
 */
int sys_mbox_valid(sys_mbox_t *mbox)
{
    return (int)(*mbox);
}
#endif

#ifndef sys_mbox_set_invalid
/** Set an mbox invalid so that sys_mbox_valid returns 0
 */
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
    *mbox = NULL;
}
#endif

/* ====================== System ====================== */

/*
 * Start a new thread named "name" with priority "prio" that will begin
 * its execution in the function "thread()". The "arg" argument will be
 * passed as an argument to the thread() function
 */
sys_thread_t sys_thread_new(const char    *name,
                            lwip_thread_fn thread,
                            void          *arg,
                            int            stacksize,
                            int            prio)
{
    Thread *t;

    COS_DEBUG_NOT_IN_INTERRUPT;

    /* create thread */
    t = new Thread(name, thread, arg, stacksize, prio, 20);

    COS_ASSERT(t != NULL);

    /* startup thread */
    t->startup();

    return t;
}

sys_prot_t sys_arch_protect(void)
{
    base_t level;

    /* disable interrupt */
    level = arch_interrupt_disable();

    return level;
}

void sys_arch_unprotect(sys_prot_t pval)
{
    /* enable interrupt */
    arch_interrupt_enable(pval);

    return;
}

void sys_arch_assert(const char *file, int line)
{
    printk("\nAssertion: %d in %s, thread %s\n",
           line, file, Scheduler::get_current_thread()->name());
    COS_ASSERT(0);
}

u32_t sys_jiffies(void)
{
    return tick_get();
}

u32_t sys_now(void)
{
    return tick_get() * (1000 / CONFIG_TICK_PER_SECOND);
}

u32_t sio_read(sio_fd_t fd, u8_t *buf, u32_t size)
{
    u32_t len;

    COS_ASSERT(fd != NULL);

    len = ((Device *)fd)->read(0, buf, size);
    if (len <= 0)
        return 0;

    return len;
}

u32_t sio_write(sio_fd_t fd, u8_t *buf, u32_t size)
{
    COS_ASSERT(fd != NULL);
    return ((Device *)fd)->write(0, buf, size);
}

void sio_read_abort(sio_fd_t fd)
{
    printk("read_abort\n");
}

extern Device *_console_device;
void ppp_trace(int level, const char *format, ...)
{
    va_list args;
    size_t length;
    static char cos_log_buf[CONFIG_CONSOLEBUF_SIZE];

    va_start(args, format);
    length = vsnprintf(cos_log_buf, sizeof(cos_log_buf) - 1, format, args);
    if(_console_device != NULL) {
        _console_device->write(0, cos_log_buf, length);
    }
    va_end(args);
}


