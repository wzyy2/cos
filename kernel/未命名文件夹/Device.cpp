#include "Device.h"


Device::Device()
{

}

Device::~Device()
{

}

err_t register_device(Device *dev, char *name, uint16_t flags)
{
    if (dev == NULL)
        return -ERR_ERROR;
    if (find(name) != NULL)
        return -ERR_ERROR;

    Object::init();

    flag_ = flags
    ref_count_ = 0;
}

err_t unregister_device(Device *dev)
{
    COS_ASSERT(dev != NULL);

    return ERR_OK;
}

Device *find()
{



}