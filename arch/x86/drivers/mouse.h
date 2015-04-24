#ifndef MOUSE_H
#define MOUSE_H

#include <cos/device/device.h>

class Mouse : public Device
{
public:
    Mouse(const char *name);
    ~Mouse();

private:
    unsigned char mouse_cycle_ = 0;     //unsigned char
    signed char mouse_byte_[3];    //signed char
    signed char mouse_x_ = 0;         //signed char
    signed char mouse_y_ = 0;         //signed char

    static void isr(int vector, void *param);
    void wait(uint8_t a_type);
    void write(uint8_t a_write);
    uint8_t read();
    void install();
};

#endif // MOUSE_H
