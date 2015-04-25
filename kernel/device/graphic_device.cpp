/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "cos/device/graphic_device.h"

#include <arch/arch.h>
#include <cos/cos.h>


GraphicDeivce::GraphicDeivce(const char *name) : Device(Device::Device_Class_Graphic,
                                                        name, Device::FLAG_RDWR | Device::FLAG_STANDALONE)
{
    bits_per_pixel_ = 0;
    pixel_format_ = PIXEL_FORMAT_MONO;
    framebuffer_ = NULL;
    width_ = 0;
    height_ = 0;
}

GraphicDeivce::~GraphicDeivce()
{

}

err_t GraphicDeivce::driver_control(uint8_t cmd, void *args)
{
    err_t result = -ERR_IO;

    switch (cmd)
    {
    case CTRL_GET_WIDTH:
        *(uint16_t *)args = width_;
        result = ERR_OK;
        break;
    case CTRL_GET_HEIGHT:
        *(uint16_t *)args = height_;
        result = ERR_OK;
        break;
    case CTRL_GET_FRAME:
        *(uint8_t *)args = bits_per_pixel_;
        result = ERR_OK;
        break;
    case CTRL_GET_FORMAT:
        *(color_type *)args = pixel_format_;
        result = ERR_OK;
        break;
    case CTRL_GET_BITS_PER_PIXEL:
        *(uint8_t **)args = framebuffer_;
        result = ERR_OK;
        break;

    case CTRL_RECT_UPDATE:
        /* nothong to be done */
        break;
    default:
        break;
    }

    return result;
};
