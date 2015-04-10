#ifndef TIMER_H
#define TIMER_H

#include <cos/cosDef.h>
#include <cos/object.h>


/**
 * @addtogroup Clock
 */

/*@{*/

/**
 * clock & timer macros
 */
#define TIMER_FLAG_DEACTIVATED       0x0             /**< timer is deactive */
#define TIMER_FLAG_ACTIVATED         0x1             /**< timer is active */
#define TIMER_FLAG_ONE_SHOT          0x0             /**< one shot timer */
#define TIMER_FLAG_PERIODIC          0x2             /**< periodic timer */

#define TIMER_FLAG_HARD_TIMER        0x0             /**< hard timer,the timer's callback function will be called in tick isr. */
#define TIMER_FLAG_SOFT_TIMER        0x4             /**< soft timer,the timer's callback function will be called in timer thread. */

#define TIMER_CTRL_SET_TIME          0x0             /**< set timer control command */
#define TIMER_CTRL_GET_TIME          0x1             /**< get timer control command */
#define TIMER_CTRL_SET_ONESHOT       0x2             /**< change timer to one shot */
#define TIMER_CTRL_SET_PERIODIC      0x3             /**< change timer to periodic */


/**
 * Class of timer
 */
class Timer : public Object
{
public:
    Timer();
    ~Timer();

private:
    void (*timeout_func_)(void *parameter);              /**< timeout function */
    void            *parameter_;                         /**< timeout function's parameter */

    tick_t        init_tick_;                         /**< timer timeout tick */
    tick_t        timeout_tick_;                      /**< timeout tick */
};

/*@}*/

#endif // TIMER_H
