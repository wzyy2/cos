/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "serial.h"

#include <arch/arch.h>
#include <cos/cos.h>

Serial::Serial(const char *name, ubase_t com) : Device(Device::Device_Class_Char,
                                                       name, Device::FLAG_RDWR | Device::FLAG_INT_RX | Device::FLAG_STREAM)
{
    PORT_ = com;
    if(PORT_ == COM2 || PORT_ == COM4){
        interrupt_ = INTCOM2;
    } else {
        interrupt_ = INTCOM1;
    }

    init_serial();
}

Serial::~Serial()
{
    arch_interrupt_mask(interrupt_);
}

void Serial::init_serial()
{
    outb(PORT_ + 1, 0x00);    // Disable all interrupts
    outb(PORT_ + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT_ + 0, 0x01);    // Set divisor to 1 (lo byte) 115200 baud

    outb(PORT_ + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT_ + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT_ + 4, 0x0B);    // IRQs enabled, RTS/DSR set

    arch_interrupt_install(interrupt_, (isr_handler_t) Serial::isr, this);
    arch_interrupt_umask(interrupt_);

    outb(PORT_ + 1, 0x01);    // Data available interruptsc
}

int Serial::received()
{
    return inb(PORT_ + COMSTATUS) & 1;
}

char Serial::sgetc()
{
    while (received() == 0);

    return inb(PORT_);
}

int Serial::is_transmit_empty()
{
    return inb(PORT_ + COMSTATUS) & 0x20;
}

void Serial::printc(char a)
{
    while (is_transmit_empty() == 0);

    if(a == '\n' && newline_){
        //CRLF
        outb(PORT_,'\n');
        outb(PORT_,'\r');
    } else {
        outb(PORT_,a);
    }
}

size_t Serial::driver_write(off_t pos, const void * buffer, size_t size)
{
    size_t i = size;
    const char* str = static_cast<const char*> (buffer);

    while(i--)
    {
        printc(*str++);
    }

    return size;
}

size_t Serial::driver_read(off_t pos, void* buffer, size_t size)
{
    uint8_t* ptr = static_cast<uint8_t*> (buffer);
    err_t err_code = ERR_OK;

    /* interrupt mode Rx */
    while (size)
    {
        base_t level;

        /* disable interrupt */
        level = arch_interrupt_disable();

        if (read_index_ != save_index_)
        {
            /* read a character */
            *ptr++ = rx_buffer_[read_index_];
            size--;

            /* move to next position */
            read_index_ ++;
            if (read_index_ >= SERIAL_RX_BUFFER_SIZE)
                read_index_ = 0;
        }
        else
        {
            /* set error code */
            err_code = -ERR_EMPTY;

            /* enable interrupt */
            arch_interrupt_enable(level);
            break;
        }

        /* enable interrupt */
        arch_interrupt_enable(level);
    }

    /* set error code */
    set_errno(err_code);
    return (uint32_t)ptr - (uint32_t)buffer;
}

void Serial::isr(int vector, void *param)
{
    char c;
    base_t level;

    Serial *serial = (Serial *) param;

    c = serial->sgetc();

    /* disable interrupt */
    level = arch_interrupt_disable();

    /* save character */
    serial->rx_buffer_[serial->save_index_] = c;
    serial->save_index_ ++;
    if (serial->save_index_ >= SERIAL_RX_BUFFER_SIZE)
        serial->save_index_ = 0;

    /* if the next position is read index, discard this 'read char' */
    if (serial->save_index_ == serial->read_index_)
    {
        serial->read_index_ ++;
        if (serial->read_index_ >= SERIAL_RX_BUFFER_SIZE)
            serial->read_index_ = 0;
    }

    /* enable interrupt */
    arch_interrupt_enable(level);
}
