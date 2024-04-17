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

    auto se=split("./home/test/x",'/');
    for(int i=0;i<se.size();i++)
        std::cout<<se.at(i)<<' '<<'\n';

    std::cout<<se[0]<<'\n';

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
            //std::cout<<i<<'\n';
            Buf* bp= br_mgr.Bread(0,i);
            io_move( (char*)&superblock.s_nfree,bp->b_addr,sizeof(superblock.s_nfree));

            io_move ((char *)&superblock.s_free,bp->b_addr+sizeof(superblock.s_nfree),sizeof(superblock.s_free));


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
        io_move( (char *)(&superblock)+i*BLOCK_SIZE, bp->b_addr,BLOCK_SIZE);

        br_mgr.not_avaible(bp);
        br_mgr.get_device_manager()->get_blk_device()->Strategy(bp);
        br_mgr.Bwrite(bp);
    }

    Inode root_dir_node=alloc_inode();

    int root_dir_blk=alloc_blk();

    DirItem new_dir_blk[MAX_DIR_NUM];
    strcpy(new_dir_blk[0].name,".");
    new_dir_blk[0].inode_no= root_dir_node.i_number;

    // write the block of root dir into disk
    Buf * bp=br_mgr.Bread(0,root_dir_blk);
    io_move( (char *)(&new_dir_blk), bp->b_addr,BLOCK_SIZE);
    br_mgr.not_avaible(bp);
    br_mgr.get_device_manager()->get_blk_device()->Strategy(bp);
    br_mgr.Bwrite(bp);

    root_dir_node.i_nlink=1;
    root_dir_node.i_uid=root_dir_node.i_gid=0;
    root_dir_node.i_size= sizeof(DirItem);
    root_dir_node.i_addr[0]=root_dir_blk;
    root_dir_node.i_atime= (unsigned int)(time(NULL));
    root_dir_node.i_mtime= (unsigned int) (time(NULL));

    std::cout<<root_dir_node.i_number<<' '<<root_dir_blk<<'\n';

    save_inode(root_dir_node);

    create_dir("./etc",0,0,0);

    create_dir("./bin",0,0,0);

    create_dir("./home",0,0,0);

    create_dir("./dev",0,0,0);

    







    
    



    return;
}

void FileSystem::io_move(char * src, char * dst, unsigned int nbytes)
{
    // for(int i=0;i<nbytes;i++)
    //     dst[i]=src[i];
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
    if(superblock.s_block_free_num<=0)
    {
        std::cerr<<"[ERROR]fail to allocate memory resouce.\n";
        return -1;
    }

    int re=0;
    superblock.s_block_free_num-=1;

    re=superblock.s_free[--superblock.s_nfree];
    if(superblock.s_nfree==0 && superblock.s_block_free_num!=0) // there are more free blks, but the stack will be empty
    {
        Buf * bp= br_mgr.Bread(0,superblock.s_free[0]);
        io_move(bp->b_addr, (char*)(&superblock.s_nfree),sizeof(superblock.s_nfree));
        io_move(bp->b_addr+sizeof(superblock.s_nfree),(char*)superblock.s_free,sizeof(superblock.s_free));
    }

    return re;
}

void FileSystem::save_inode(Inode inode)
{
    DiskInode d;
    memcpy(&d, &inode, INODE_SIZE);


    int blk_no = inode.i_number/(BLOCK_SIZE/INODE_SIZE)+2 ;
    Buf * bp=br_mgr.Bread(0,blk_no);
    io_move( (char *)(&d), bp->b_addr+blk_no*BLOCK_SIZE+ inode.i_number%(BLOCK_SIZE/INODE_SIZE)*INODE_SIZE,BLOCK_SIZE);
    br_mgr.not_avaible(bp);
    br_mgr.get_device_manager()->get_blk_device()->Strategy(bp);
    br_mgr.Bwrite(bp);


}

Inode FileSystem::load_inode(int inode_no)
{
    DiskInode tmp;
    int blk_no = inode_no/(BLOCK_SIZE/INODE_SIZE)+2 ; // each blk has 8 disk_inode
    std::cout<<"debug load inode"<<' '<<blk_no<<'\n';

    Buf* bp=br_mgr.Bread(0,blk_no);

    io_move((char*)&tmp, bp->b_addr+blk_no*BLOCK_SIZE+ inode_no%(BLOCK_SIZE/INODE_SIZE)*INODE_SIZE,INODE_SIZE);

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

void FileSystem::create_dir(const char * dir_name, short u_id,short g_id,int cur_dir_no)
{
    std::vector<std::string> paths=split(dir_name,'/');
    std::string last_dir_name=paths.at(paths.size()-1);

    if (last_dir_name.size()>=MAX_NAME_SIZE)
    {
        std::cerr<<"the length of new directory is too long.\n";
        return;
    }

    Inode cur_dir_node=load_inode(cur_dir_no);

    for(unsigned int i=0;i<paths.size()-1;i++)
        cur_dir_node=search(cur_dir_node, paths.at(i).data());

}

std::vector<std::string> FileSystem::split(const std::string & str, char delimiter)
{
    std::vector<std::string> substrings;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delimiter)) 
    {
        substrings.push_back(token);
    }
    return substrings;
}


Inode FileSystem:: search (Inode cur_dir_inode, const char * name)
{
    if(cur_dir_inode.i_flag & inode_flag::DIR_FILE ==0)
    {
        std::cerr<<name<<" is not directory.\n";
        return Inode();
    }

    DirItem items[MAX_DIR_NUM];

    

}


FileSystem::FileSystem(/* args */)
{
}

FileSystem::~FileSystem()
{

}