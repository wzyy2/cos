#ifndef _COS_H
#define _COS_H

#include <cos/cosDebug.h>
#include <cos/cosConf.h>
#include <cos/cosDef.h>

#include <arch/arch.h>

#include <cos/irq.h>
#include <cos/sysheap.h>
#include <cos/kservice.h>
#include <cos/init.h>
#include <cos/clock.h>


#ifdef __cplusplus
#include <cos/runtime.h>
#include <cos/scheduler.h>
#include <cos/thread.h>
#include <cos/device.h>
#include <cos/timer.h>
#include <cos/idle.h>
#include <cos/ipc.h>
#include <cos/semaphore.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

#endif /* _COS_H */
