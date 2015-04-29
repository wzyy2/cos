#ifndef ETH_DEVICE_H
#define ETH_DEVICE_H

#include <cos/device/device.h>
#include <cos/ipc/semaphore.h>

#define NIOCTL_GADDR		0x01
#define ETHERNET_MTU		1500

#ifdef CONFIG_NET

#include "lwip/netif.h"

class EthDevice : public Device
{
public:
    EthDevice(const char *name);
    ~EthDevice();

private:
    /* network interface for lwip */
    struct netif *netif;
    Semaphore *tx_ack;

    uint8_t  flags_;
    uint8_t  link_changed_;
    uint16_t link_status_;

    struct pbuf* (*eth_rx)();
    err_t (*eth_tx)(struct pbuf* p);

};

#endif

#endif // ETH_DEVICE_H
