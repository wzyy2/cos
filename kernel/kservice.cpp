#include "cos/kservice.h"

#include <cos/cos.h>

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
    Thread *thread;
    err_t ret;

    if (interrupt_get_nest() != 0)
    {
        /* it's in interrupt context */
        ret = _errno;
        _errno = ERR_OK;
        return ret;
    }
    thread = Scheduler::get_current_thread();

    if (thread == NULL){
        ret = _errno;
        _errno = ERR_OK;
        return ret;
    } else {
        ret = thread->error_;
        thread->error_ = ERR_OK;
        return ret;
    }
}

/*
 * This function will set errno
 *
 * @param error the errno shall be set
 */
void set_errno(err_t error)
{
    Thread *thread;

    if (interrupt_get_nest() != 0)
    {
        /* it's in interrupt context */
        _errno = error;

        return;
    }

     thread = Scheduler::get_current_thread();
     if (thread == NULL)
     {
        _errno = error;

         return;
     }

     thread->error_ = error;
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

    Thread *thread = Scheduler::get_current_thread();
    if (thread != NULL)
        return (int *)&(thread->error_);

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
/* private function */
#define isdigit(c)  ((unsigned)((c) - '0') < 10)

inline int32_t divide(int32_t *n, int32_t base)
{
    int32_t res;

    /* optimized for processor which does not support divide instructions. */
    if (base == 10)
    {
        res = ((int32_t)*n) % 10U;
        *n = ((int32_t)*n) / 10U;
    }
    else
    {
        res = ((int32_t)*n) % 16U;
        *n = ((int32_t)*n) / 16U;
    }

    return res;
}

inline int skip_atoi(const char **s)
{
    register int i=0;
    while (isdigit(**s))
        i = i * 10 + *((*s)++) - '0';

    return i;
}
#define ZEROPAD     (1 << 0)    /* pad with zero */
#define SIGN        (1 << 1)    /* unsigned/signed long */
#define PLUS        (1 << 2)    /* show plus */
#define SPACE       (1 << 3)    /* space if plus */
#define LEFT        (1 << 4)    /* left justified */
#define SPECIAL     (1 << 5)    /* 0x */
#define LARGE       (1 << 6)    /* use 'ABCDEF' instead of 'abcdef' */

static char *print_number(char *buf,
                          char *end,
                          long  num,
                          int   base,
                          int   s,
                          int   precision,
                          int   type)
{
    char c, sign;
    char tmp[32];

    const char *digits;
    static const char small_digits[] = "0123456789abcdef";
    static const char large_digits[] = "0123456789ABCDEF";
    register int i;
    register int size;

    size = s;

    digits = (type & LARGE) ? large_digits : small_digits;
    if (type & LEFT)
        type &= ~ZEROPAD;

    c = (type & ZEROPAD) ? '0' : ' ';

    /* get sign */
    sign = 0;
    if (type & SIGN)
    {
        if (num < 0)
        {
            sign = '-';
            num = -num;
        }
        else if (type & PLUS)
            sign = '+';
        else if (type & SPACE)
            sign = ' ';
    }

    if (type & SPECIAL)
    {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }


    i = 0;
    if (num == 0)
        tmp[i++]='0';
    else
    {
        while (num != 0)
            tmp[i++] = digits[divide(&num, base)];
    }

    if (i > precision)
        precision = i;
    size -= precision;

    if (!(type&(ZEROPAD | LEFT)))
    {
        if ((sign)&&(size>0))
            size--;

        while (size-->0)
        {
            if (buf <= end)
                *buf = ' ';
            ++ buf;
        }
    }

    if (sign)
    {
        if (buf <= end)
        {
            *buf = sign;
            -- size;
        }
        ++ buf;
    }

    if (type & SPECIAL)
    {
        if (base==8)
        {
            if (buf <= end)
                *buf = '0';
            ++ buf;
        }
        else if (base == 16)
        {
            if (buf <= end)
                *buf = '0';
            ++ buf;
            if (buf <= end)
            {
                *buf = type & LARGE? 'X' : 'x';
            }
            ++ buf;
        }
    }

    /* no align to the left */
    if (!(type & LEFT))
    {
        while (size-- > 0)
        {
            if (buf <= end)
                *buf = c;
            ++ buf;
        }
    }

    while (i < precision--)
    {
        if (buf <= end)
            *buf = '0';
        ++ buf;
    }

    /* put number in the temporary buffer */
    while (i-- > 0)
    {
        if (buf <= end)
            *buf = tmp[i];
        ++ buf;
    }

    while (size-- > 0)
    {
        if (buf <= end)
            *buf = ' ';
        ++ buf;
    }

    return buf;
}

int32_t _vsnprintf(char       *buf,
                        size_t   size,
                        const char *fmt,
                        va_list     args)
{
    unsigned long long num;
    int i, len;
    char *str, *end, c;
    const char *s;

    uint8_t base;            /* the base of number */
    uint8_t flags;           /* flags to print number */
    uint8_t qualifier;       /* 'h', 'l', or 'L' for integer fields */
    int32_t field_width;     /* width of output field */

    int precision;      /* min. # of digits for integers and max for a string */

    str = buf;
    end = buf + size - 1;

    /* Make sure end is always >= buf */
    if (end < buf)
    {
        end  = ((char *)-1);
        size = end - buf;
    }

    for (; *fmt ; ++fmt)
    {
        if (*fmt != '%')
        {
            if (str <= end)
                *str = *fmt;
            ++ str;
            continue;
        }

        /* process flags */
        flags = 0;

        while (1)
        {
            /* skips the first '%' also */
            ++ fmt;
            if (*fmt == '-') flags |= LEFT;
            else if (*fmt == '+') flags |= PLUS;
            else if (*fmt == ' ') flags |= SPACE;
            else if (*fmt == '#') flags |= SPECIAL;
            else if (*fmt == '0') flags |= ZEROPAD;
            else break;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt)) field_width = skip_atoi(&fmt);
        else if (*fmt == '*')
        {
            ++ fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.')
        {
            ++ fmt;
            if (isdigit(*fmt)) precision = skip_atoi(&fmt);
            else if (*fmt == '*')
            {
                ++ fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0) precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = 0;

        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
        {
            qualifier = *fmt;
            ++ fmt;
            if (qualifier == 'l' && *fmt == 'l')
            {
                qualifier = 'L';
                ++ fmt;
            }
        }

        /* the default base */
        base = 10;

        switch (*fmt)
        {
        case 'c':
            if (!(flags & LEFT))
            {
                while (--field_width > 0)
                {
                    if (str <= end) *str = ' ';
                    ++ str;
                }
            }

            /* get character */
            c = (uint8_t)va_arg(args, int);
            if (str <= end) *str = c;
            ++ str;

            /* put width */
            while (--field_width > 0)
            {
                if (str <= end) *str = ' ';
                ++ str;
            }
            continue;

        case 's':
            s = va_arg(args, char *);
            if (!s) s = "(NULL)";

            len = strlen(s);
            if (precision > 0 && len > precision) len = precision;

            if (!(flags & LEFT))
            {
                while (len < field_width--)
                {
                    if (str <= end) *str = ' ';
                    ++ str;
                }
            }

            for (i = 0; i < len; ++i)
            {
                if (str <= end) *str = *s;
                ++ str;
                ++ s;
            }

            while (len < field_width--)
            {
                if (str <= end) *str = ' ';
                ++ str;
            }
            continue;

        case 'p':
            if (field_width == -1)
            {
                field_width = sizeof(void *) << 1;
                flags |= ZEROPAD;
            }

            str = print_number(str, end,
                               (long)va_arg(args, void *),
                               16, field_width, precision, flags);

            continue;

        case '%':
            if (str <= end) *str = '%';
            ++ str;
            continue;

            /* integer number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            if (str <= end) *str = '%';
            ++ str;

            if (*fmt)
            {
                if (str <= end) *str = *fmt;
                ++ str;
            }
            else
            {
                -- fmt;
            }
            continue;
        }

        if (qualifier == 'L') num = va_arg(args, long long);
        else if (qualifier == 'l')
        {
            num = va_arg(args, uint32_t);
            if (flags & SIGN) num = (int32_t)num;
        }
        else if (qualifier == 'h')
        {
            num = (uint16_t)va_arg(args, int32_t);
            if (flags & SIGN) num = (int16_t)num;
        }
        else
        {
            num = va_arg(args, uint32_t);
            if (flags & SIGN) num = (int32_t)num;
        }
        str = print_number(str, end, num, base, field_width, precision, flags);
    }

    if (str <= end) *str = '\0';
    else *end = '\0';

    /* the trailing null byte doesn't count towards the total
    * ++str;
    */
    return str - buf;
}

/**
 * This function will print a formatted string on system console
 *
 * @param fmt the format
 */
char _log_buf[CONFIG_CONSOLEBUF_SIZE];
size_t _length = 0;
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

    if(_console_device != NULL) {
        if(_length !=0){
            _console_device->write(0, _log_buf, _length);
            _length = 0;
        }
        _console_device->write(0, cos_log_buf, length);
    } else{
         memcpy( _log_buf + _length,cos_log_buf, length);
         _length += length;
    }


    va_end(args);
}
