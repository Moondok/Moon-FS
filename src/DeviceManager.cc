#include <DeviceManager.h>

BlockDevice * DeviceManager:: get_blk_device()
{
    return bdevsw;
}

DeviceManager::DeviceManager()
{
    bdevsw=new BlockDevice();    
}

DeviceManager :: ~DeviceManager()
{
    if(bdevsw!=nullptr)
        delete bdevsw;
}