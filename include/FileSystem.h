#ifndef __FILE_SYSTEM__
#define __FILE_SYSTEM__

#include <DiskInode.h>
#include <SuperBlock.h>


class FileSystem
{
private:
    /* data */
public:
    FileSystem(/* args */);

    void initialize(); //initialize the filesystem, when the server boots

    void format();      // format the system


    ~FileSystem();
};




#endif











