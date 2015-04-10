#ifndef _DEVICE_H
#define _DEVICE_H

#include <cos/cosDef.h>
#include <cos/object.h>

/**
 * device flags defitions
 */
#define DEVICE_FLAG_DEACTIVATE       0x000           /**< device is not not initialized */

#define DEVICE_FLAG_RDONLY           0x001           /**< read only */
#define DEVICE_FLAG_WRONLY           0x002           /**< write only */
#define DEVICE_FLAG_RDWR             0x003           /**< read and write */

#define DEVICE_FLAG_REMOVABLE        0x004           /**< removable device */
#define DEVICE_FLAG_STANDALONE       0x008           /**< standalone device */
#define DEVICE_FLAG_ACTIVATED        0x010           /**< device is activated */
#define DEVICE_FLAG_SUSPENDED        0x020           /**< device is suspended */
#define DEVICE_FLAG_STREAM           0x040           /**< stream mode */

#define DEVICE_FLAG_INT_RX           0x100           /**< INT mode on Rx */
#define DEVICE_FLAG_DMA_RX           0x200           /**< DMA mode on Rx */
#define DEVICE_FLAG_INT_TX           0x400           /**< INT mode on Tx */
#define DEVICE_FLAG_DMA_TX           0x800           /**< DMA mode on Tx */

#define DEVICE_OFLAG_CLOSE           0x000           /**< device is closed */
#define DEVICE_OFLAG_RDONLY          0x001           /**< read only access */
#define DEVICE_OFLAG_WRONLY          0x002           /**< write only access */
#define DEVICE_OFLAG_RDWR            0x003           /**< read and write */
#define DEVICE_OFLAG_OPEN            0x008           /**< device is opened */

/**
 * general device commands
 */
#define DEVICE_CTRL_RESUME           0x01            /**< resume device */
#define DEVICE_CTRL_SUSPEND          0x02            /**< suspend device */

/**
 * special device commands
 */
#define DEVICE_CTRL_CHAR_STREAM      0x10            /**< stream mode on char device */
#define DEVICE_CTRL_BLK_GETGEOME     0x10            /**< get geometry information   */
#define DEVICE_CTRL_BLK_SYNC         0x11            /**< flush data to block device */
#define DEVICE_CTRL_BLK_ERASE        0x12            /**< erase block on block device */
#define DEVICE_CTRL_BLK_AUTOREFRESH  0x13            /**< block device : enter/exit auto refresh mode */
#define DEVICE_CTRL_NETIF_GETMAC     0x10            /**< get mac address */
#define DEVICE_CTRL_MTD_FORMAT       0x10            /**< format a MTD device */
#define DEVICE_CTRL_RTC_GET_TIME     0x10            /**< get time */
#define DEVICE_CTRL_RTC_SET_TIME     0x11            /**< set time */
#define DEVICE_CTRL_RTC_GET_ALARM    0x12            /**< get alarm */
#define DEVICE_CTRL_RTC_SET_ALARM    0x13            /**< set alarm */


class Device : public Object
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

    Device(device_class_type type, const char *name, uint16_t flags);
    ~Device();

    static Device *find(const char *name);
    err_t init ();
    err_t open(uint16_t oflag);
    err_t close();
    err_t control(uint8_t cmd, void *args);
    size_t write(off_t pos, const void * buffer, size_t size);
    size_t read(off_t pos, void* buffer, size_t size);  

    err_t set_rx_indicate(err_t (*rx_ind)(Device *dev, size_t size));
    err_t set_tx_complete(err_t (*tx_done)(Device *dev, void *buffer));

protected:
    virtual err_t driver_init(){
        return ERR_OK;
    };
    virtual err_t driver_open(uint16_t oflag){
        return ERR_OK;
    };
    virtual err_t driver_close(){
        return ERR_OK;
    };
    virtual err_t driver_control(uint8_t cmd, void *args){
        return ERR_OK;
    };
    virtual size_t driver_write(off_t pos, const void *buffer, size_t size){
        return 0;
    };
    virtual size_t driver_read(off_t pos, void *buffer, size_t size){
        return 0;
    };


    err_t (*rx_indicate_)(Device *dev, size_t size) = NULL;
    err_t (*tx_complete_)(Device *dev, void *buffer) = NULL;

    void  *user_data;                /**< device private data */
private:
    device_class_type type_;        /**< device type */
    //uint16_t    flag_;                     /**< device flag */
    uint16_t    open_flag_;                /**< device open flag */

    uint8_t ref_count_;                /**< reference count */
    uint8_t device_id_;                /**< 0 - 255 */

};


#endif //_DEVICE_H
