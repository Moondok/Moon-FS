#ifndef __SUPER_BLOCK__
#define __SUPER_BLOCK__

class SuperBlock// 1024 bytes
{
public:
    int s_inode_num; // the number of diskinodes
    int s_inode_free_num;
    int s_block_num; // the number of data blocks
    int s_block_free_num;

    int s_nfree; // empty data blocks pointer
    int s_free[100]; // index of empty blocks
    int s_flock;

    int s_ninode; // empty disk inodes pointer
    int s_inode[100]; // index of empty disk inodes
    int s_ilock=0;

    int fmod;

    int ronly;
    int s_time;

    int padding[45];


};

#endif