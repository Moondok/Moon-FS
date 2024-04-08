#ifndef __FILE_SYSTEM__
#define __FILE_SYSTEM__

#include <DiskInode.h>
#include <SuperBlock.h>
#include <fstream>
#include <iostream>
#include <BufferManager.h>

class FileSystem
{
private:
    /* data */
    SuperBlock superblock;
public:
    FileSystem(/* args */);

    void initialize(); //initialize the filesystem, when the server boots

    void format();      // format the system


    ~FileSystem();
};




#endif











