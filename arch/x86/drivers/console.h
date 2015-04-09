#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <cos/device.h>

class Console : public Device
{
public:
    static Console *Instance();
    static bool Destroy();

    char keyboard_getc(void);
    void cga_init(void);
    void cga_putc(int c);
    void printc(int c);

    err_t driver_init();
    size_t driver_write(off_t pos, const void * buffer, size_t size);
    size_t driver_read(off_t pos, void* buffer, size_t size);
    static void isr(int vector);

    void hw_init(void);
    void hw_output(const char* str);


private:
    static Console *self;

    unsigned addr_6845_;
    uint16_t *crt_buf_;
    int16_t  crt_pos_;

    static const unsigned int CONSOLE_RX_BUFFER_SIZE = 128;
    uint8_t  rx_buffer_[Console::CONSOLE_RX_BUFFER_SIZE];
    uint32_t read_index_, save_index_;

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
