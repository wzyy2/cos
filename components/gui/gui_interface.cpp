/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include "gui_interface.h"

#include <arch/arch.h>
#include <cos/cos.h>
#include "video.h"
#include "mouse.h"

#include "GUI.h"
#include "FRAMEWIN.h"
#include <math.h>


void _DemoTransWindow(void);
Video *video;

void set_pixel(const char *pixel, int x, int y)
{
    video->set_pixel(pixel, x, y);
}

void get_pixel (char *pixel, int x, int y)
{
    video->get_pixel(pixel, x, y);
}


void draw_hline (const char *pixel, int x1, int x2, int y)
{
    video->draw_hline(pixel, x1, x2, y);
}

void draw_vline (const char *pixel, int x, int y1, int y2)
{
    video->draw_vline(pixel, x, y1, y2);
}


void blit_line (const char *pixel, int x, int y, size_t size)
{
    video->blit_line(pixel, x, y, size);
}

static err_t mouse_rx(Device *dev, size_t size)
{
    Mouse *mouse = (Mouse *) dev;
    int cycle;
    if(mouse->mouse_cycle_ == 0){
        cycle = 2;
    } else {
        cycle = mouse->mouse_cycle_ - 1;
    }
    GUI_MOUSE_DRIVER_PS2_OnRx(mouse->mouse_byte_[cycle]);

    return ERR_OK;
}

ALIGN(4)
static uint8_t thread_stack[1024 * 32];

void gui_entry(void *p)
{
    GUI_SetBkColor(GUI_BLUE); //设置背景颜色
    GUI_SetColor(GUI_WHITE); //设置前景颜色，及字体和绘图的颜色
    GUI_Clear(); //按指定颜色清屏
    GUI_DispStringAt("Hello World ..",10,10); //显示字符
    GUI_CURSOR_Show();     //显示鼠标,测试触摸屏     必须打开窗口功能 GUI_WINSUPPORT

    Mouse *mouse = (Mouse *) Device::find("mouse");

    mouse->set_rx_indicate(mouse_rx);

    while(1) {
        Thread::sleep(CONFIG_TICK_PER_SECOND/50);
        GUI_Exec();
    }
}

void cos_gui_init(void)
{
    video = (Video *) Device::find("video");
    GUI_Init();

    Thread *gui = new Thread("gui", gui_entry, NULL, &thread_stack[0] \
            , sizeof(thread_stack), 0, 30);
    gui->startup();
}

INIT_APP_EXPORT(cos_gui_init);
