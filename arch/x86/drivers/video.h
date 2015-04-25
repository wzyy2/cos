#ifndef _VIDEO_H
#define _VIDEO_H

#include <cos/device/graphic_device.h>

class Video : public GraphicDeivce
{
public:
    Video(const char *name, uint16_t width,
          uint16_t height, uint8_t bits, color_type format, uint8_t *frame);
    ~Video();

    void set_pixel(const char *pixel, int x, int y);
    void get_pixel (char *pixel, int x, int y);

    void draw_hline (const char *pixel, int x1, int x2, int y);
    void draw_vline (const char *pixel, int x, int y1, int y2);

    void blit_line (const char *pixel, int x, int y, size_t size);

private:

};


#endif
