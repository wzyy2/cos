#include <cos.h>


/**
 * @addtogroup KernelService
 */

/*@{*/

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

    //rt_hw_console_output(rt_log_buf);

    va_end(args);
}


/*@{*/