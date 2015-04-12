#ifndef TIMER_H
#define TIMER_H

#include <cos/cosDef.h>
#include <cos/object.h>


/**
 * Class of timer
 */
class Timer : public Object
{
public:
    Timer(const char *name,
          void (*timeout)(void *parameter),
          void      *parameter,
          tick_t  time,
          uint8_t flag);
    ~Timer();

    err_t detach();
    err_t start();
    err_t stop();
    err_t control(uint8_t cmd, void *arg);
    static void check(void);

    /**
     * clock & timer macros
     */
    static const uint8_t FLAG_DEACTIVATED   =    0x0; /**< timer is deactive */
    static const uint8_t FLAG_ACTIVATED     =    0x1; /**< timer is active */
    static const uint8_t FLAG_ONE_SHOT      =    0x0; /**< one shot timer */
    static const uint8_t FLAG_PERIODIC      =    0x2; /**< periodic timer */

    static const uint8_t FLAG_HARD_TIMER    =    0x0; /**< hard timer,the timer's callback function will be called in tick isr. */
    static const uint8_t FLAG_SOFT_TIMER    =    0x4; /**< soft timer,the timer's callback function will be called in timer thread. */

    static const uint8_t CTRL_SET_TIME      =    0x0; /**< set timer control command */
    static const uint8_t CTRL_GET_TIME      =    0x1; /**< get timer control command */
    static const uint8_t CTRL_SET_ONESHOT   =    0x2; /**< change timer to one shot */
    static const uint8_t CTRL_SET_PERIODIC  =    0x3; /**< change timer to periodic */

private:
    void (*timeout_func_)(void *parameter);              /**< timeout function */
    void            *parameter_;                         /**< timeout function's parameter */

    tick_t        init_tick_;                         /**< timer timeout tick */
    tick_t        timeout_tick_;                      /**< timeout tick */

    struct Compare{
        bool operator () (const Timer *p1, const Timer *p2) const
        {
            //small to big
            return p1->timeout_tick_ < p2->timeout_tick_;
        }
    };

    static std::set<Timer *, Compare> timer_set_;  /**< timeout set */
};



#endif // TIMER_H
