#ifndef __BUFFER_MANAGER_H__
#define __BUFFER_MANAGER_H__

#include <util.h>


enum BufFlag
{
    B_WRITE=0x1,
    B_READ=0x2,
    B_DONE=0x4, //IO operation over,the data is ready
    B_ERROR=0x8, // some errors occurs in I/O
    B_BUSY=0x10, 
    B_WANTED =0x20,
    B_ASYNC=0x40,
    B_DELWRI=0x80,

};


// Buf is the controller of one block
class Buf 
{
    short b_dev;
    int b_blk_no;
    unsigned char * b_addr; //point to the buffer memory it controls
    unsigned int b_flags; //
    int b_error;
    int b_resid;
    Buf* b_forw; //pointer used in the linked list : all buffer blocks of this device
    Buf* b_back;
    Buf* av_forw; //pointer used in the linked list : available buffer blocks
    Buf* av_back; 


};

class BufferManager
{
private:
    static const int NUM_BUFFER=15;
    static const int BUFFER_SIZE= 512;

    Buf m_Buf[NUM_BUFFER];

    unsigned char Buffer [NUM_BUFFER][BUFFER_SIZE];

    Buf bFreeList; // the head of available buffer linked list
    Buf ioList; 



public:
    BufferManager();


    ~BufferManager();
};





#endif