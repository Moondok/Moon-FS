#ifndef __FILE_SYSTEM__
#define __FILE_SYSTEM__

#include <DiskInode.h>
#include <SuperBlock.h>
#include <fstream>
#include <iostream>
#include <BufferManager.h>
#include <cstring>
#include <Inode.h>
#include <ctime>
#include <string>
#include <vector>
#include <sstream>

class FileSystem
{
private:
    /* data */
    SuperBlock superblock;
    BufferManager br_mgr;

    Inode inode_table[20];
public:
    FileSystem(/* args */);

    void initialize(); //initialize the filesystem, when the server boots

    void format();      // format the system

    void io_move(char * src,char * dst, unsigned int nbyte);// transfer data between user space and buffer

    Inode alloc_inode();

    void save_inode(Inode inode);

    int alloc_blk();

    Inode load_inode(int inode_no);


    void create_dir(const char * dir_name,short u_id, short g_id,int cur_dir_no);


    std::vector<std::string> split(const std::string & str, char delimiter);

    Inode search(Inode cur_inode, const char *name);


    void read_(Inode & inode, char * buf, unsigned int start, unsigned int len);
    void write_(Inode &inode, char * buf, unsigned int start, unsigned int len);

    ~FileSystem();
};




#endif











