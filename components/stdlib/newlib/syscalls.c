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
_execve_r(struct _reent *ptr, const char * name, char *const *argv, char *const *env)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_fcntl_r(struct _reent *ptr, int fd, int cmd, int arg)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_fork_r(struct _reent *ptr)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_fstat_r(struct _reent *ptr, int fd, struct stat *pstat)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_getpid_r(struct _reent *ptr)
{
	return 0;
}

int
_isatty_r(struct _reent *ptr, int fd)
{
	if (fd >=0 && fd < 3) return 1;

	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_kill_r(struct _reent *ptr, int pid, int sig)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_link_r(struct _reent *ptr, const char *old, const char *new)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

_off_t
_lseek_r(struct _reent *ptr, int fd, _off_t pos, int whence)
{
	return 0;
}

int
_mkdir_r(struct _reent *ptr, const char *name, int mode)
{
	return 0;
}

int
_open_r(struct _reent *ptr, const char *file, int flags, int mode)
{

	return 0;

}

_ssize_t 
_read_r(struct _reent *ptr, int fd, void *buf, size_t nbytes)
{

	return 0;

}

int
_rename_r(struct _reent *ptr, const char *old, const char *new)
{

	return 0;

}

void *
_sbrk_r(struct _reent *ptr, ptrdiff_t incr)
{
	/* no use this routine to get memory */
	return NULL;
}

int
_stat_r(struct _reent *ptr, const char *file, struct stat *pstat)
{

	return 0;

}

_CLOCK_T_
_times_r(struct _reent *ptr, struct tms *ptms)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

int
_unlink_r(struct _reent *ptr, const char *file)
{

	return 0;

}

int
_wait_r(struct _reent *ptr, int *status)
{
	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}

_ssize_t
_write_r(struct _reent *ptr, int fd, const void *buf, size_t nbytes)
{

        return 0;

}



#ifndef MILLISECOND_PER_SECOND
#define MILLISECOND_PER_SECOND	1000UL
#endif

#ifndef MICROSECOND_PER_SECOND
#define MICROSECOND_PER_SECOND	1000000UL
#endif

#ifndef NANOSECOND_PER_SECOND
#define NANOSECOND_PER_SECOND	1000000000UL
#endif

#define MILLISECOND_PER_TICK	(MILLISECOND_PER_SECOND / RT_TICK_PER_SECOND)
#define MICROSECOND_PER_TICK	(MICROSECOND_PER_SECOND / RT_TICK_PER_SECOND)
#define NANOSECOND_PER_TICK		(NANOSECOND_PER_SECOND  / RT_TICK_PER_SECOND)


struct timeval _timevalue = {0};

static void libc_system_time_init(void)
{

}


int libc_get_time(struct timespec *time)
{


	return 0;
}

int
_gettimeofday_r(struct _reent *ptr, struct timeval *__tp, void *__tzp)
{

	/* return "not supported" */
	ptr->_errno = ENOTSUP;
	return -1;
}


/* Memory routine */
void *
_malloc_r (struct _reent *ptr, size_t size)
{
	return -1;
}

void *
_realloc_r (struct _reent *ptr, void *old, size_t newlen)
{
	return -1;
}

void *_calloc_r (struct _reent *ptr, size_t size, size_t len)
{
	return -1;
}

void 
_free_r (struct _reent *ptr, void *addr)
{
	return -1;
}

void
_exit (int status)
{

	printf("thread exit\n");
	COS_ASSERT(0);

	while (1);
}

void 
_system(const char *s)
{
    /* not support this call */
    return;
}
