#ifndef __BUFFER_MANAGER_H__
#define __BUFFER_MANAGER_H__

#include <util.h>
#include <DeviceManager.h>



class BufferManager
{
private:
    static const int NUM_BUFFER=15;
    static const int BUFFER_SIZE= 512;

    Buf m_Buf[NUM_BUFFER];

    unsigned char Buffer [NUM_BUFFER][BUFFER_SIZE];

    Buf bFreeList; // the head of available buffer linked list
    

    DeviceManager * m_DeviceManager;



public:

    Buf * get_blk(int dev_no,int blk_no);

    void Bwrite(Buf * bp);

    Buf * Bread(int dev_no,int blk_no);

    DeviceManager * get_device_manager();

    void not_avaible(Buf* bp); //pick a blk out of freelist


    BufferManager();


    ~BufferManager();
};





#endif