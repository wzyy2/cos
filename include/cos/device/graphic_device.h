#ifndef GRAPHIC_DEVICE
#define GRAPHIC_DEVICE

#include <cos/device/device.h>

class GraphicDeivce : public Device
{
public:
    GraphicDeivce(const char *name);
    ~GraphicDeivce();

    static const uint8_t  CTRL_CURSOR_SET_POSITION  = 0x10;        /**< cursor control command */
    static const uint8_t  CTRL_CURSOR_SET_TYPE    =  0x11;         /**< cursor control command */

    /**
     * graphic device control command
     */
    static const uint8_t    CTRL_RECT_UPDATE    =  0;
    static const uint8_t    CTRL_POWERON        =  1;
    static const uint8_t    CTRL_POWEROFF       =  2;
    static const uint8_t    CTRL_SET_MODE       =  3;
    static const uint8_t    CTRL_GET_EXT        =  4;
    static const uint8_t    CTRL_GET_WIDTH       =  5;
    static const uint8_t    CTRL_GET_HEIGHT       =  6;
    static const uint8_t    CTRL_GET_FRAME       =  7;
    static const uint8_t    CTRL_GET_FORMAT       =  8;
    static const uint8_t    CTRL_GET_BITS_PER_PIXEL       =  9;

    err_t driver_control(uint8_t cmd, void *args);

    /* graphic deice */
    enum color_type
    {
        PIXEL_FORMAT_MONO = 0,
        PIXEL_FORMAT_GRAY4,
        PIXEL_FORMAT_GRAY16,
        PIXEL_FORMAT_RGB332,
        PIXEL_FORMAT_RGB444,
        PIXEL_FORMAT_RGB565,
        PIXEL_FORMAT_RGB565P,
        PIXEL_FORMAT_BGR565 = PIXEL_FORMAT_RGB565P,
        PIXEL_FORMAT_RGB666,
        PIXEL_FORMAT_RGB888,
        PIXEL_FORMAT_ARGB888
    };

    virtual void set_pixel(const char *pixel, int x, int y) = 0;
    virtual void get_pixel (char *pixel, int x, int y) = 0;

    virtual void draw_hline (const char *pixel, int x1, int x2, int y) = 0;
    virtual void draw_vline (const char *pixel, int x, int y1, int y2) = 0;

    virtual void blit_line (const char *pixel, int x, int y, size_t size) = 0;

protected:
    color_type  pixel_format_;                           /**< graphic format */
    uint8_t  bits_per_pixel_;                         /**< bits per pixel */
    uint16_t reserved_;                               /**< reserved field */

    uint16_t width_;                                  /**< width of graphic device */
    uint16_t height_;                                 /**< height of graphic device */

    uint8_t *framebuffer_;                            /**< frame buffer */

};
#endif // GRAPHIC_DEVICE

