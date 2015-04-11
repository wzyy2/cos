#include "cos/kservice.h"

#include <cos/cos.h>
#include <cos/cosHw.h>

#include <stdio.h>

static Device *_console_device = NULL;

/* global errno in cos */
static volatile int _errno;

/**
 * This function will show the version of cos rtos
 */

void show_version(void)
{
    printk("\n _____   _____   _____  \n");
    printk("/  ___| /  _  \\ /  ___/ \n");
    printk("| |     | | | | | |___  \n");
    printk("| |     | | | | \\___  \\ \n");
    printk("| |___  | |_| |  ___| | \n");
    printk("\\_____| \\_____/ /_____/ \n");
    printk(" %d.%d.%d build %s\n",
               C_VERSION, C_SUBVERSION, C_REVISION, __DATE__);
    printk(" 2015 - 2021 Copyright by Jacob Chen \n");
}

/*
 * This function will get errno and clear errno
 *
 * @return errno
 */
err_t get_errno(void)
{
    // rt_thread_t tid;
    err_t ret;

    if (interrupt_get_nest() != 0)
    {
        /* it's in interrupt context */
        ret = _errno;
    }
    ret = _errno;
    // tid = rt_thread_self();
    // if (tid == RT_NULL)

    _errno = ERR_OK;
    return ret;
    // return tid->error;
}

/*
 * This function will set errno
 *
 * @param error the errno shall be set
 */
void set_errno(err_t error)
{
    // rt_thread_t tid;

    if (interrupt_get_nest() != 0)
    {
        /* it's in interrupt context */
        _errno = error;

        return;
    }

    // tid = rt_thread_self();
    // if (tid == RT_NULL)
    // {
        _errno = error;

    //     return;
    // }

    // tid->error = error;
}

/**
 * This function returns errno.
 *
 * @return the errno in the system
 */
int *cos_errno(void)
{
    if (interrupt_get_nest() != 0)
        return (int *)&_errno;

    return (int *)&_errno;
}

/**
 * This function returns the device using in console.
 *
 * @return the device using in console or NULL
 */
Device *console_get_device(void)
{
    return _console_device;
}

/**
 * This function will set a device as console device.
 * After set a device to console, all output of printk will be
 * redirected to this new device.
 *
 * @param name the name of new console device
 *
 * @return the old console device handler
 */
Device *console_set_device(const char *name)
{
    Device *newd, *old;

    /* save old device */
    old = _console_device;

    /* find new console device */
    newd = Device::find(name);
    if (newd != NULL)
    {
        if (_console_device != NULL)
        {
            /* close old console device */
            _console_device->close();
        }

        /* set new console device */
        _console_device = newd;
        _console_device->open(Device::OFLAG_RDWR | Device::FLAG_STREAM);
    }

    return old;
}


/**
 * This function will print a formatted string on system console
 *
 * @param fmt the format
 */
void printk(const char *fmt, ...)
{
    va_list args;
    size_t length;
    static char cos_log_buf[CONFIG_CONSOLEBUF_SIZE];

    va_start(args, fmt);
    /* the return value of vsnprintf is the number of bytes that would be
     * written to buffer had if the size of the buffer been sufficiently
     * large excluding the terminating null byte. If the output string
     * would be larger than the rt_log_buf, we have to adjust the output
     * length. */
    length = vsnprintf(cos_log_buf, sizeof(cos_log_buf) - 1, fmt, args);
    if (length > CONFIG_CONSOLEBUF_SIZE - 1)
        length = CONFIG_CONSOLEBUF_SIZE - 1;

    if(_console_device != NULL)
        _console_device->write(0, cos_log_buf, length);

    va_end(args);
}
