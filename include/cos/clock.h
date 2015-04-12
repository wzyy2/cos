#ifndef CLOCK_H
#define CLOCK_H

#include <cos/cosConf.h>
#include <cos/cosDef.h>


#ifdef __cplusplus
extern "C" {
#endif

#define  MS_TICK  (1000UL / CONFIG_TICK_PER_SECOND)
#define  US_TICK  (1000000UL / CONFIG_TICK_PER_SECOND)
#define  NS_TICK  (1000000000UL / CONFIG_TICK_PER_SECOND)

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

