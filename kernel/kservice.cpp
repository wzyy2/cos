#include "cos/kservice.h"

#include <cos/cos.h>
#include <cos/cosHw.h>

#include <stdio.h>

static Device *_console_device = NULL;

/* global errno in cos */
static volatile int _errno;

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
        _console_device->open(DEVICE_OFLAG_RDWR | DEVICE_FLAG_STREAM);
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



//int32_t  vsnprintf(char       *buf,
//                        size_t   size,
//                        const char *fmt,
//                        va_list     args)
//{

//    unsigned long long num;

//    int i, len;
//    char *str, *end, c;
//    const char *s;

//    uint8_t base;            /* the base of number */
//    uint8_t flags;           /* flags to print number */
//    uint8_t qualifier;       /* 'h', 'l', or 'L' for integer fields */
//    int32_t field_width;     /* width of output field */

//    str = buf;
//    end = buf + size - 1;

//    /* Make sure end is always >= buf */
//    if (end < buf)
//    {
//        end  = ((char *)-1);
//        size = end - buf;
//    }

//    for (; *fmt ; ++fmt)
//    {
//        if (*fmt != '%')
//        {
//            if (str <= end)
//                *str = *fmt;
//            ++ str;
//            continue;
//        }

//        /* process flags */
//        flags = 0;

//        while (1)
//        {
//            /* skips the first '%' also */
//            ++ fmt;
//            if (*fmt == '-') flags |= LEFT;
//            else if (*fmt == '+') flags |= PLUS;
//            else if (*fmt == ' ') flags |= SPACE;
//            else if (*fmt == '#') flags |= SPECIAL;
//            else if (*fmt == '0') flags |= ZEROPAD;
//            else break;
//        }

//        /* get field width */
//        field_width = -1;
//        if (isdigit(*fmt)) field_width = skip_atoi(&fmt);
//        else if (*fmt == '*')
//        {
//            ++ fmt;
//            /* it's the next argument */
//            field_width = va_arg(args, int);
//            if (field_width < 0)
//            {
//                field_width = -field_width;
//                flags |= LEFT;
//            }
//        }

//        /* get the conversion qualifier */
//        qualifier = 0;

//        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')

//        {
//            qualifier = *fmt;
//            ++ fmt;
//            if (qualifier == 'l' && *fmt == 'l')
//            {
//                qualifier = 'L';
//                ++ fmt;
//            }
//        }

//        /* the default base */
//        base = 10;

//        switch (*fmt)
//        {
//        case 'c':
//            if (!(flags & LEFT))
//            {
//                while (--field_width > 0)
//                {
//                    if (str <= end) *str = ' ';
//                    ++ str;
//                }
//            }

//            /* get character */
//            c = (uint8_t)va_arg(args, int);
//            if (str <= end) *str = c;
//            ++ str;

//            /* put width */
//            while (--field_width > 0)
//            {
//                if (str <= end) *str = ' ';
//                ++ str;
//            }
//            continue;

//        case 's':
//            s = va_arg(args, char *);
//            if (!s) s = "(NULL)";

//            len = rt_strlen(s);

//            if (!(flags & LEFT))
//            {
//                while (len < field_width--)
//                {
//                    if (str <= end) *str = ' ';
//                    ++ str;
//                }
//            }

//            for (i = 0; i < len; ++i)
//            {
//                if (str <= end) *str = *s;
//                ++ str;
//                ++ s;
//            }

//            while (len < field_width--)
//            {
//                if (str <= end) *str = ' ';
//                ++ str;
//            }
//            continue;

//        case 'p':
//            if (field_width == -1)
//            {
//                field_width = sizeof(void *) << 1;
//                flags |= ZEROPAD;
//            }

//            str = print_number(str, end,
//                               (long)va_arg(args, void *),
//                               16, field_width, flags);

//            continue;

//        case '%':
//            if (str <= end) *str = '%';
//            ++ str;
//            continue;

//            /* integer number formats - set up the flags and "break" */
//        case 'o':
//            base = 8;
//            break;

//        case 'X':
//            flags |= LARGE;
//        case 'x':
//            base = 16;
//            break;

//        case 'd':
//        case 'i':
//            flags |= SIGN;
//        case 'u':
//            break;

//        default:
//            if (str <= end) *str = '%';
//            ++ str;

//            if (*fmt)
//            {
//                if (str <= end) *str = *fmt;
//                ++ str;
//            }
//            else
//            {
//                -- fmt;
//            }
//            continue;
//        }

//        if (qualifier == 'L')
//            num = va_arg(args, long long);
//        else if (qualifier == 'l')
//        {
//            num = va_arg(args, uint32_t);
//            if (flags & SIGN) num = (int32_t)num;
//        }
//        else if (qualifier == 'h')
//        {
//            num = (rt_uint16_t)va_arg(args, int32_t);
//            if (flags & SIGN) num = (rt_int16_t)num;
//        }
//        else
//        {
//            num = va_arg(args, uint32_t);
//            if (flags & SIGN) num = (int32_t)num;
//        }

//        str = print_number(str, end, num, base, field_width, flags);

//    }

//    if (str <= end) *str = '\0';
//    else *end = '\0';

//    /* the trailing null byte doesn't count towards the total
//    * ++str;
//    */
//    return str - buf;
//}
