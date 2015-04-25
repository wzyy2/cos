/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "mouse.h"

#include <arch/arch.h>
#include <cos/cos.h>

Mouse::Mouse(const char *name) : Device(Device::Device_Class_Char,
                         name, Device::FLAG_RDWR | Device::FLAG_INT_RX)
{
    install();
}

Mouse::~Mouse()
{
    arch_interrupt_mask(INTMOUSE);
}

/**
 * Mouse functions
 */
void Mouse::isr(int vector, void *param) //struct regs *a_r (not used but just there)
{
    Mouse *mouse = (Mouse *) param;
    switch(mouse->mouse_cycle_)
    {
    case 0:
        mouse->mouse_byte_[0] = inb(0x60);
        mouse->mouse_cycle_++;
        break;
    case 1:
        mouse->mouse_byte_[1] = inb(0x60);
        mouse->mouse_cycle_++;
        break;
    case 2:
        mouse->mouse_byte_[2] = inb(0x60);
        mouse->mouse_x_ = mouse->mouse_byte_[1];
        mouse->mouse_y_ = mouse->mouse_byte_[2];
        mouse->mouse_cycle_ = 0;
        break;
    }

    /* invoke callback */
    if (mouse->rx_indicate_ != NULL)
    {
        mouse->rx_indicate_(mouse, 0);
    }

}

void Mouse::wait(uint8_t a_type) //unsigned char
{
    unsigned int _time_out=100000; //unsigned int
    if(a_type==0)
    {
        while(_time_out--) //Data
        {
            if((inb(0x64) & 1)==1)
            {
                return;
            }
        }
        return;
    }
    else
    {
        while(_time_out--) //Signal
        {
            if((inb(0x64) & 2)==0)
            {
                return;
            }
        }
        return;
    }
}

void Mouse::write(uint8_t a_write) //unsigned char
{
    //Wait to be able to send a command
    wait(1);
    //Tell the mouse we are sending a command
    outb(0x64, 0xD4);
    //Wait for the final part
    wait(1);
    //Finally write
    outb(0x60, a_write);
}

uint8_t Mouse::read()
{
    //Get's response from mouse
    wait(0);
    return inb(0x60);
}

void Mouse::install()
{
    uint8_t _status;  //unsigned char

    //Setup the mouse handler
    arch_interrupt_install(INTMOUSE, (isr_handler_t) Mouse::isr, this);
    arch_interrupt_umask(INTMOUSE);

    //Enable the auxiliary mouse device
    wait(1);
    outb(0x64, 0xA8);

    //Enable the interrupts
    wait(1);
    outb(0x64, 0x20);
    wait(0);
    _status=(inb(0x60) | 2);
    wait(1);
    outb(0x64, 0x60);
    wait(1);
    outb(0x60, _status);
    wait(1);
    //Tell the mouse to use default settings
    write(0xF6);
    read();  //Acknowledge

    //Enable the mouse
    write(0xF4);
    read();  //Acknowledge


}

static void mouse_init()
{
    Mouse *mouse = new Mouse("mouse");
    if(mouse == NULL || get_errno() != ERR_OK){
        printk("ERROR : Fail to create mouse!\n");
        delete mouse;
    }
}

INIT_DEVICE_EXPORT(mouse_init);
