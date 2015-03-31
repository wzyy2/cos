#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <cos.h>
#include <Device.h>

class Console : public Device
{
public:
    static Console *Instance();
    static bool Destroy();

    char keyboard_getc(void);
    void cga_init(void);
    void cga_putc(int c);
    void putc(int c);

    err_t init (dev_t dev);
    err_t open(dev_t dev, uint16_t oflag);
    err_t close(dev_t dev);
    err_t control(dev_t dev, uint8_t cmd, void *args);
    size_t write(dev_t dev, off_t pos, const void * buffer, size_t size);
    size_t read(dev_t dev, off_t pos, void* buffer, size_t size);
    void isr(int vector);

    void hw_init(void);
    void hw_output(const char* str);


private:
    unsigned addr_6845_;
    uint16_t *crt_buf_;
    int16_t  crt_pos_;

    Console();
    ~Console();

    static int shiftcode[256];

    static int togglecode[256];

    const static char normalmap[256];

    const static char shiftmap[256]; 

    const static char ctlmap[256];

    const static char *charcode[4];
};

#endif //_CONSOLE_H