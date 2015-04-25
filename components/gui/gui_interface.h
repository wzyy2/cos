/*
 * (C) 2015 Copyright by Jacob Chen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef GUI_INTERFACE_H
#define GUI_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

void set_pixel(const char *pixel, int x, int y);
void get_pixel(char *pixel, int x, int y);

void draw_hline(const char *pixel, int x1, int x2, int y);
void draw_vline(const char *pixel, int x, int y1, int y2);

void blit_line(const char *pixel, int x, int y, unsigned int  size);
void cos_gui_init(void);


#ifdef __cplusplus
}
#endif

#endif // GUI_INTERFACE_H

