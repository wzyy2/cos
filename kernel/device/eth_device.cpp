#include "cos/device/eth_device.h"

#ifdef CONFIG_NET

EthDevice::EthDevice(const char *name) : Device(Device::Device_Class_NetIf,
                                name, Device::FLAG_RDWR | Device::FLAG_STANDALONE)
{

}

EthDevice::~EthDevice()
{

}

#endif
