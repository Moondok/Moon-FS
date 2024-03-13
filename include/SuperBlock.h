#ifndef __SUPER_BLOCK__
#define __SUPER_BLOCK__

class SuperBlock// 1024 bytes
{
public:
    int s_size; // the number of diskinodes
    int s_fsize; // the number of data blocks

    int s_nfree; // empty data blocks
    int s_free[100]; // index of empty blocks
    int s_flock;

    int s_ninode; // empty disk inodes 
    int s_inode[100]; // index of empty disk inodes
    int s_ilock;

    int fmod;

    int ronly;
    int s_time;

    int padding[47];


};

#endif