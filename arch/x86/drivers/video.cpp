/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "video.h"

#include <arch/arch.h>
#include <cos/cos.h>



Video::Video(const char *name, uint16_t width,
             uint16_t height, uint8_t bits, color_type format, uint8_t *frame) : GraphicDeivce(name)
{
    bits_per_pixel_ = bits;
    pixel_format_ = format;
    framebuffer_ = frame;
    width_ = width;
    height_ = height;
}

Video::~Video()
{

}

void Video::set_pixel(const char *pixel, int x, int y)
{
    uint8_t bt = bits_per_pixel_ / 8;
    unsigned long where = x * bt + y * width_ * bt;
    while(bt) {
        framebuffer_[where] = *(pixel);
        pixel++;
        where++;
        bt--;
    }
}

void Video::get_pixel (char *pixel, int x, int y)
{
    uint8_t bt = bits_per_pixel_ / 8;
    unsigned long where = x * bt + y * width_ * bt;
    while(bt) {
        *(pixel) = framebuffer_[where];
        pixel++;
        where++;
        bt--;
    }
}


void Video::draw_hline (const char *pixel, int x1, int x2, int y)
{
    for(; x1<x2; x1++)
    {
        set_pixel(pixel, x1, y);
    }
}

void Video::draw_vline (const char *pixel, int x, int y1, int y2)
{
    for(; y1<y2; y1++)
    {
        set_pixel(pixel, x, y1);
    }
}


void Video::blit_line (const char *pixel, int x, int y, size_t size)
{
    uint8_t bt = bits_per_pixel_ / 8;
    unsigned long where = x * bt + y * width_ * bt;
    while(size--) {
        for(int i = 0; i < bt; i++) {
            framebuffer_[where] = pixel[i];
            where++;
        }
    }
}

static void video_init()
{
    uint8_t BT = (display_info->pitch / display_info->Xres);
    Video::color_type color_type = Video::PIXEL_FORMAT_MONO;
    if(BT == 2){
        color_type = Video::PIXEL_FORMAT_RGB565;
    } else if(BT == 3){
        color_type = Video::PIXEL_FORMAT_RGB888;
    } else if(BT == 4){
        color_type = Video::PIXEL_FORMAT_ARGB888;
    }

    Video *video = new Video("video", display_info->Xres, display_info->Yres,
                             BT * 8, color_type,
                             (uint8_t *)display_info->physbase);

    if(video == NULL || get_errno() != ERR_OK){
        printk("ERROR : Fail to create video!\n");
        delete video;
    }
}

INIT_DEVICE_EXPORT(video_init);
