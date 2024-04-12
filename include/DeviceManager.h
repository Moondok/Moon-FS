# ifndef __DEVICE_MANAGER_H__
#define  __DEVICE_MANAGER_H__

#include <BlockDevice.h>

class DeviceManager
{
private:
    int num_blk_device;//num of types of blk device
    BlockDevice * bdevsw ;//we only has one type of block device 

public:
    BlockDevice * get_blk_device();

    DeviceManager();

    ~DeviceManager();

};

#endif