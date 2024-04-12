#ifndef __FILE_SYSTEM__
#define __FILE_SYSTEM__

#include <DiskInode.h>
#include <SuperBlock.h>
#include <fstream>
#include <iostream>
#include <BufferManager.h>
#include <cstring>

class FileSystem
{
private:
    /* data */
    SuperBlock superblock;
    BufferManager br_mgr;
public:
    FileSystem(/* args */);

    void initialize(); //initialize the filesystem, when the server boots

    void format();      // format the system

    void io_move(const char * src,const char * dst, unsigned int nbyte);// transfer data between user space and buffer

    ~FileSystem();
};




#endif











