#ifndef _DEVICE_H
#define _DEVICE_H

#include <cos.h>

#include "Object.h"

/**
 * @addtogroup Device
 */

/*@{*/

class Device ： public Object
{
public:
    /**
     * device (I/O) class type
     */
    enum device_class_type
    {
        Device_Class_Char = 0,                           /**< character device */
        Device_Class_Block,                              /**< block device */
        Device_Class_NetIf,                              /**< net interface */
        Device_Class_MTD,                                /**< memory device */
        Device_Class_CAN,                                /**< CAN device */
        Device_Class_RTC,                                /**< RTC device */
        Device_Class_Sound,                              /**< Sound device */
        Device_Class_Graphic,                            /**< Graphic device */
        Device_Class_I2CBUS,                             /**< I2C bus device */
        Device_Class_USBDevice,                          /**< USB slave device */
        Device_Class_USBHost,                            /**< USB host bus */
        Device_Class_SPIBUS,                             /**< SPI bus device */
        Device_Class_SPIDevice,                          /**< SPI device */
        Device_Class_SDIO,                               /**< SDIO bus device */
        Device_Class_PM,                                 /**< PM pseudo device */
        Device_Class_Pipe,                               /**< Pipe device */
        Device_Class_Portal,                             /**< Portal device */
        Device_Class_Miscellaneous,                      /**< Miscellaneous device */
        Device_Class_Unknown                             /**< unknown device */
    };

    static err_t register_device(Device *dev, char *name, uint16_t flags);

    static err_t unregister_device(Device *dev);
    static Device *find();


    Device();
    ~Device();

    uint16_t flag_;

protected:
    virtual err_t init () = 0;
    virtual err_t open(uint16_t oflag) = 0;
    virtual err_t close() = 0;
    virtual err_t control(uint8_t cmd, void *args) = 0;
    virtual size_t write(off_t pos, const void * buffer, size_t size) = 0;
    virtual size_t read(off_t pos, void* buffer, size_t size) = 0;
    virtual void isr(int vector) = 0;   


    enum device_class_type type_;        /**< device type */
    uint16_t    flag_;                     /**< device flag */
    uint16_t    open_flag_;                /**< device open flag */

    uint8_t ref_count_;                /**< reference count */
    uint8_t device_id_;                /**< 0 - 255 */

};

/*@}*/
#endif //_DEVICE_H