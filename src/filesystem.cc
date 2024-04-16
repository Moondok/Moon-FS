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

    memset(superblock.s_free,0,sizeof(superblock.s_free));

    for(int i=superblock.s_block_num-1;i>=0;i--)
    {
        if(superblock.s_nfree==100)
        {
            
            Buf* bp= br_mgr.Bread(0,i);
            io_move( (char*)&superblock.s_nfree,bp->b_addr,sizeof(superblock.s_nfree));

            io_move ((char *)&superblock.s_free,bp->b_addr+sizeof(superblock),sizeof(superblock.s_free));


            br_mgr.not_avaible(bp);
            br_mgr.get_device_manager()->get_blk_device()->Strategy(bp);
            br_mgr.Bwrite(bp);

            superblock.s_nfree=0;
            memset(superblock.s_free,0,sizeof(superblock.s_free));
        }

        superblock.s_free[superblock.s_nfree++]=i;
    }

    //copy the superblock to disk
    //std::cout<<sizeof(superblock)<<'\n';

    //write the superblock to the disk
    for(int i=0;i<2;i++)
    {
        Buf * bp=br_mgr.Bread(0,i);
        io_move( (char *)(&superblock+i*BLOCK_SIZE), bp->b_addr,BLOCK_SIZE);

        br_mgr.not_avaible(bp);
        br_mgr.get_device_manager()->get_blk_device()->Strategy(bp);
        br_mgr.Bwrite(bp);
    }


    
    



    return;
}

void FileSystem::io_move(char * src, char * dst, unsigned int nbytes)
{
    memcpy(dst,src,nbytes);

    
}

Inode FileSystem::alloc_inode()
{
    if(superblock.s_inode_free_num<=0)
    {
        std::cerr<<"[ERROR]no free inode\n";
        return Inode();
    }
    else if(superblock.s_ninode<=0) // there r still free inodes, just the stack is empty
    {
        // we only need allocate 100 inodes,
        for(int i=0;i<superblock.s_inode_num && superblock.s_ninode<100;i++)
        {
            Inode inode=load_inode(i);
            if(inode.i_mode & inode_flag::IALLOC ==0) //un allocated diskinode
                superblock.s_inode[superblock.s_ninode++]=i;

        }
    }
    superblock.s_inode_free_num-=1;

    int re=superblock.s_inode[--superblock.s_ninode];

    return load_inode(re);



}

int FileSystem::alloc_blk()
{
    return 0;
}

Inode FileSystem::load_inode(int inode_no)
{
    DiskInode tmp;
    int blk_no = inode_no/(BLOCK_SIZE/INODE_SIZE) ; // each blk has 8 disk_inode

    Buf* bp=br_mgr.Bread(0,blk_no);

    io_move((char*)&tmp, bp->b_addr+inode_no%(BLOCK_SIZE/INODE_SIZE),INODE_SIZE);

    Inode return_inode;
    return_inode.i_mode=tmp.d_mode;
    return_inode.i_nlink=tmp.d_nlink;
    return_inode.i_uid=tmp.d_uid;
    return_inode.i_gid=tmp.d_gid;
    return_inode.i_size=tmp.d_size;

    memcpy(return_inode.i_addr, tmp.d_addr,sizeof(int)*10);
    return_inode.i_atime=tmp.d_atime;
    return_inode.i_mtime=tmp.d_mtime;

    return_inode.i_dev=0;
    return_inode.i_number=inode_no;

    return return_inode;



}

FileSystem::FileSystem(/* args */)
{
}

FileSystem::~FileSystem()
{

}