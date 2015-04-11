#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <cos/cosDef.h>
#include <cos/ipc.h>


/**
 * Class of Semaphore
 */
class Semaphore : public IPC
{
public:
    Semaphore(const char *name, uint32_t value, uint8_t  flag);
    ~Semaphore();

    err_t detach();
    err_t take(int32_t time);
    err_t trytake();
    err_t release();
    err_t control(uint8_t cmd, void *arg);


private:

    /**< value of semaphore. */
    uint16_t value_;

};

/*@}*/

#endif // SEMAPHORE_H
