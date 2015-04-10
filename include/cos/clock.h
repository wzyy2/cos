#ifndef CLOCK_H
#define CLOCK_H

#include <cos/cosConf.h>
#include <cos/cosDef.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * clock & timer interface
 */

tick_t tick_get(void);
void tick_set(tick_t tick);
void tick_increase(void);
tick_t tick_from_millisecond(uint32_t ms);


#ifdef __cplusplus
}
#endif

#endif // CLOCK_H

