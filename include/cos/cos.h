#ifndef _COS_H
#define _COS_H

#include <cos/cosDebug.h>
#include <cos/cosConf.h>
#include <cos/cosDef.h>


#include <cos/irq.h>
#include <cos/memheap.h>
#include <cos/kservice.h>



#ifdef __cplusplus

#include <cos/device.h>

#include "Video.h"
extern Video vid;

extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif





#endif /* _COS_H */
