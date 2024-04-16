#ifndef __INODE_H__
#define __INODE_H__

#include<util.h>

enum inode_flag
{
    IALLOC=0x1,
    DIR_FILE=0x2,


};

class Inode
{
public:
    unsigned int i_mode;
    unsigned int i_nlink;
    short i_uid;
    short i_gid;
    unsigned int  i_size ; // number of valid bytes
    int i_addr[10];
    unsigned int i_atime;
    unsigned int i_mtime;

    short i_dev;
    short i_number;

    unsigned int i_flag;
    unsigned int i_count;
};


class DiskInode
{
public:
    unsigned int d_mode;
    unsigned int d_nlink;
    short d_uid;
    short d_gid;
    unsigned int d_size;
    int d_addr[10];
    unsigned int d_atime;
    unsigned int d_mtime;
};

class DirItem
{
public: 
    char name[28];
    int inode_no;
};



#endif