/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <reent.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <cos/cos.h>

/* Reentrant versions of system calls.  */

int
_close_r(struct _reent *ptr, int fd)
{
    return 0;
}

int
_fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
    /* return "not supported" */
    ptr->_errno = ENOTSUP;
    return -1;
}

int
_isatty_r(struct _reent *ptr, int fd)
{
    if (fd >=0 && fd < 3) return 1;

    /* return "not supported" */
    ptr->_errno = ENOTSUP;
    return -1;
}

_off_t
_lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
    return 0;
}

_ssize_t
_read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{

    return 0;

}


_ssize_t
_write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{

        return 0;

}

/* Memory routine */
void *
_malloc_r (struct _reent *ptr, size_t size)
{
    void* result;

    result = (void*)kmalloc (size);
    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void *
_realloc_r (struct _reent *ptr, void *old, size_t newlen)
{
    void* result;

    result = (void*)krealloc (old, newlen);
    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void *_calloc_r (struct _reent *ptr, size_t size, size_t len)
{
    void* result;

    result = (void*)kcalloc (size, len);
    if (result == NULL)
    {
        ptr->_errno = ENOMEM;
    }

    return result;
}

void
_free_r (struct _reent *ptr, void *addr)
{
    kfree (addr);
}

void
_exit (int status)
{

    printk("thread exit\n");
    COS_ASSERT(0);

    while (1);
}

void
_system(const char *s)
{
    /* not support this call */
    return;
}
