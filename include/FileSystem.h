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
#include <File.h>
#include <iomanip>

class FileSystem
{
private:
    /* data */
    
    BufferManager br_mgr;

    Inode inode_table[20];

    



    /** user data */
    std::vector<std::string> usr_cur_dir_names;
    
    int usr_cur_dir_inode_no=0;

public:
    FileTable user_file_table[10];
    SuperBlock superblock;

    FileSystem();/* args */

    void initialize(); //initialize the filesystem, when the server boots

    void format();      // format the system

    void io_move(char * src,char * dst, unsigned int nbyte);// transfer data between user space and buffer

    Inode alloc_inode();

    void save_inode(Inode inode);

    int alloc_blk();

    Inode load_inode(int inode_no);


    void create_dir(const char * dir_name,short u_id, short g_id,int cur_dir_no);

    int create_file(const char* file_name, short u_id, short g_id, int cur_dir_no);


    std::vector<std::string> split(const std::string & str, char delimiter);

    Inode search(Inode cur_inode, const char *name);


    int read_(Inode & inode, char * buf, unsigned int start, unsigned int len);
    int write_(Inode &inode, char * buf, unsigned int start, unsigned int len);

    int change_directory( const char* file_name, short u_id, short g_id, int cur_dir_no );

    int delete_file_(Inode & inode, short u_id, short g_id);
    int delete_file(const char * file_name,  short u_id, short g_id, int cur_dir_no );


    int delete_dir_(Inode & inode, short u_id, short g_id);
    int delete_dir(const char * dir_name,  short u_id, short g_id, int cur_dir_no );

    int move(const char *src, const char * dst, short u_id, short g_id, int cur_dir_no );

    int get_usr_cur_dir_no();

    const std::vector<std::string> & get_usr_cur_names();

    void recycle_inode(int inode_no);

    void recycle_block(int block_no);
    /**** some function used by users*/
    int login_(std::string u_name, std::string u_password);

    void list(std::string  route);
    void list_(int inode_no=0);

    void check_status(const char* file_name, short u_id, short g_id, int cur_dir_no);
    
    int  open_file(const char* file_name, short u_id, short g_id, int cur_dir_no, int open_mode=File::FileFlags::FREAD);
    void    close_file(int file_no);

    void seekp(File * file_ptr, int offset, int base);

    ~FileSystem();
};




#endif











