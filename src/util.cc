#include <util.h>

const char* DISK_FILE_NAME="Disk.img";

const int BLOCK_SIZE=512; // 512 bytes

const int DISK_SIZE=131072*BLOCK_SIZE; //64MB, 131072 blocks

const int SUPER_BLOCK_SIZE=2*BLOCK_SIZE ;// 2 blocks, 1kb

const int INODE_AREA_SIZE= 1022* BLOCK_SIZE; // 1022 blocks, 8176 inodes

const int INODE_SIZE=64;

const int INODE_NUM= 8176;

const int BLOCK_NUM= 130048 ; //63.5MB

const int MAX_NAME_SIZE=28; //28 bytes, the name of directory or file

const int DIR_ITEMS_SIZE=32; //file/dir name + no

const int MAX_DIR_NUM= 16; // max : 16 dirs in one blk

const int INODE_TABLE_SIZE=20;