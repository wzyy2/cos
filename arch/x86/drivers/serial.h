#ifndef SERIAL_H
#define SERIAL_H

/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <cos/device/device.h>

class Serial : public Device
{
public:
    Serial(const char *name, ubase_t com);
    ~Serial();

    void init_serial();
    static void isr(int vector, void *param);

    int received();
    char sgetc();

    int is_transmit_empty();
    void printc(char a);

    size_t driver_read(off_t pos, void* buffer, size_t size);
    size_t driver_write(off_t pos, const void * buffer, size_t size);

    bool newline_ = true;

private:
    ubase_t PORT_;
    ubase_t interrupt_;

    static const unsigned int SERIAL_RX_BUFFER_SIZE = 256;
    uint8_t  rx_buffer_[Serial::SERIAL_RX_BUFFER_SIZE];
    uint32_t read_index_, save_index_;

};

#endif // SERIAL_H
