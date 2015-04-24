/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <cos/device/device.h>
#include "cos/cos.h"

/**
 * This function registers a device driver with specified name.
 *
 * @param name the device driver's name
 * @param flags the flag of device
 */

Device::Device(device_class_type type, const char *name, uint16_t flags)
:Object(Object::Object_Class_Device, name)
{
    type_ = type;
    flag_ = flags;
    ref_count_ = 0;
}

/**
 * This function removes a previously registered device driver
 */
Device::~Device()
{

}

/**
 * This function finds a device driver by specified name.
 *
 * @param name the device driver's name
 *
 * @return the registered device driver on successful, or NULL on failure.
 */
Device *Device::find(const char *name)
{
    Object *obj = Object::find(name, Object::Object_Class_Device);
    return (Device *) obj;
}


/**
 * This function will initialize the specified device
 *
 * @return the result
 */
err_t Device::init()
{
    err_t result = ERR_OK;

    /* get device init handler */
    if (!(flag_ & Device::FLAG_ACTIVATED))
    {
        result = driver_init();
        if (result != ERR_OK)
        {
            printk("ERROR : To initialize device:%s failed. The error code is %d\n",
                       name_, result);
        }
        else
        {
            flag_ |= Device::FLAG_ACTIVATED;
        }
    }

    return result;
}

/**
 * This function will open a device
 *
 * @param oflag the flags for device open
 *
 * @return the result
 */
err_t Device::open(uint16_t oflag)
{
    err_t result = ERR_OK;

    /* if device is not initialized, initialize it. */
    if (!(flag_ & Device::FLAG_ACTIVATED))
    {
        result = driver_init();
        if (result != ERR_OK)
        {
            printk("ERROR : To initialize device:%s failed. The error code is %d\n",
                       name_, result);
        }
        else
        {
            flag_ |= Device::FLAG_ACTIVATED;
        }
    }

    /* device is a stand alone device and opened */
    if ((flag_ & Device::FLAG_STANDALONE) &&
        (open_flag_ & Device::OFLAG_OPEN))
    {
        return -ERR_BUSY;
    }

    /* call device open interface */
    result = driver_open(oflag);

    /* set open flag */
    if (result == ERR_OK || result == -ERR_NOSYS)
    {
        open_flag_ = oflag | Device::OFLAG_OPEN;

        ref_count_++;
        /* don't let bad things happen silently. If you are bitten by this assert,
         * please set the ref_count to a bigger type. */
        COS_ASSERT(ref_count_ != 0);
    }

    return result;
}

/**
 * This function will close a device
 *
 * @return the result
 */
err_t Device::close()
{
    err_t result = ERR_OK;

    if (ref_count_ == 0)
        return -ERR_ERROR;

    ref_count_--;

    if (ref_count_ != 0)
        return ERR_OK;

    /* call device close interface */
    result = driver_close();

    /* set open flag */
    if (result == ERR_OK || result == -ERR_NOSYS)
        open_flag_ = Device::OFLAG_CLOSE;

    return result;
}

/**
 * This function will read some data from a device.
 *
 * @param pos the position of reading
 * @param buffer the data buffer to save read data
 * @param size the size of buffer
 *
 * @return the actually read size on successful, otherwise negative returned.
 *
 * @note since 0.4.0, the unit of size/pos is a block for block device.
 */
size_t Device::read(off_t    pos,
                         void   *buffer,
                         size_t   size)
{
    if (ref_count_ == 0)
    {
        set_errno(-ERR_ERROR);
        return 0;
    }

    /* call device read interface */
    return driver_read(pos, buffer, size);
}

/**
 * This function will write some data to a device.
 *
 * @param pos the position of written
 * @param buffer the data buffer to be written to device
 * @param size the size of buffer
 *
 * @return the actually written size on successful, otherwise negative returned.
 *
 * @note
 */
size_t Device::write(off_t    pos,
                          const void *buffer,
                          size_t   size)
{
    if (ref_count_ == 0)
    {
        set_errno(-ERR_ERROR);
        return 0;
    }

    /* call device write interface */
    return driver_write(pos, buffer, size);
}

/**
 * This function will perform a variety of control functions on devices.
 *
 * @param cmd the command sent to device
 * @param arg the argument of command
 *
 * @return the result
 */
err_t Device::control(uint8_t cmd, void *arg)
{
    /* call device write interface */
    return driver_control(cmd, arg);
}

/**
 * This function will set the reception indication callback function. This callback function
 * is invoked when this device receives data.
 *
 * @param dev the pointer of device
 * @param rx_ind the indication callback function
 *
 * @return ERR_OK
 */
err_t
Device::set_rx_indicate(err_t (*rx_ind)(Device *dev, size_t size))
{
    rx_indicate_ = rx_ind;

    return ERR_OK;
}

/**
 * This function will set the indication callback function when device has
 * written data to physical hardware.
 *
 * @param dev the pointer of device
 * @param tx_done the indication callback function
 *
 * @return ERR_OK
 */
err_t
Device::set_tx_complete(err_t (*tx_done)(Device *dev, void *buffer))
{
    tx_complete_ = tx_done;

    return ERR_OK;
}


