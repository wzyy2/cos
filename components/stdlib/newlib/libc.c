#include <cos/cos.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include "libc.h"


void libc_system_init(const char* tty_name)
{
	/* set PATH and HOME */
	putenv("PATH=/");
	putenv("HOME=/");
}
