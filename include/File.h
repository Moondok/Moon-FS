#ifndef __FILE_H__
#define __FILE_H__

#include <Inode.h>

class File
{
public:
    enum FileFlags
    {
        FREAD = 0x1,  //read
        FWRITE = 0x2, // write 
        FPIPE = 0x4// pipe
    };
    Inode * f_inode =nullptr;
    unsigned int f_offset =0 ;
    int f_count =0;
    int f_flag;
};

class FileTable
{
public:

    static const int NO_FILES=10;
    short u_id;
    short g_id;

    int num_files=0; // valid num of pointers
    File* files[NO_FILES];

    FileTable()
    {
        for(int i=0;i<10;i++)
            files[i]=nullptr;
    }

};





#endif