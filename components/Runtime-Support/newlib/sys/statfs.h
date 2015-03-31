#ifndef _STATFS_H
#define _STATFS_H

#include <cos.h>

struct statfs
{
	size_t f_bsize; 	 /* block size */
	size_t f_blocks;  /* total data blocks in file system */
	size_t f_bfree;	 /* free blocks in file system */
};

#endif
