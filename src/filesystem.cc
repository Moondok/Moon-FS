#include <FileSystem.h>

void FileSystem::initialize()
{
    std::ifstream in_stream;

    // note that ifstream do not create new file automatically
    in_stream.open(DISK_FILE_NAME, std::ios::binary);

    if(in_stream.is_open()==false)
        format();

    return;
}

void FileSystem::format()
{
    std::cout<<"format the filesystem ...\n";

    // create the disk file
    std::ofstream o_stream;
    o_stream.open(DISK_FILE_NAME, std::ios::binary);
    if (o_stream.is_open()==false)
        std::cerr<<"[ERROR] can not create disk file, please check your memory space.\n";

    o_stream.seekp(DISK_SIZE-1, std::ios::beg);
    o_stream.write("",sizeof(char));
    o_stream.close();


    // initialze superblock
    this->superblock.s_inode_num= INODE_NUM;
    this->superblock.s_inode_free_num=INODE_NUM;
    this->superblock.s_block_num= BLOCK_NUM;
    this->superblock.s_block_free_num= BLOCK_NUM;

        // the stack storing the empty inodes
    this->superblock.s_ninode=0;
    for(int i=99;i>=0;i--)
        this->superblock.s_inode[superblock.s_ninode++]=i;

    
        // this stack storing the empty data blocks
    this->superblock.s_nfree=1; // not zero cuz the element at the bottom points to a block



    return;
}

FileSystem::FileSystem(/* args */)
{
}

FileSystem::~FileSystem()
{

}