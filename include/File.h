#ifndef __FILE_H__
#define __FILE_H__

#include <Inode.h>

class File
{
public:
    Inode * f_inode;
    unsigned int f_offset;
    int f_count;
    int f_flag;
};

class FileTable
{
public:
    short u_id;
    short g_id;
    File* files[10];

    FileTable()
    {
        for(int i=0;i<10;i++)
            files[i]=nullptr;
    }

};





#endif