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
    // std::cout<<"format the filesystem ...\n";

    // auto se=split("./home/test/x",'/');
    // for(int i=0;i<se.size();i++)
    //     std::cout<<se.at(i)<<' '<<'\n';

    // std::cout<<se[0]<<'\n';

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

    for(int i=superblock.s_block_num+1024-1;i>=1024;i--) //super block+inode=1024 blks
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
        //int blk_no=alloc_blk();
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
    br_mgr.Bwrite(bp);   // new dir blk has error

    //Buf* bp_=br_mgr.Bread(0,)

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
    io_move( (char *)(&d), bp->b_addr+blk_no*BLOCK_SIZE+ inode.i_number%(BLOCK_SIZE/INODE_SIZE)*INODE_SIZE,INODE_SIZE);
    br_mgr.not_avaible(bp);
    br_mgr.get_device_manager()->get_blk_device()->Strategy(bp);
    br_mgr.Bwrite(bp);


    for (auto & node: inode_table)
    {
        if(node.i_number==inode.i_number)
        {
            node=inode;
            break;
        }
    }


}

Inode FileSystem::load_inode(int inode_no)
{

    for(auto i=0;i<INODE_TABLE_SIZE;i++)
    {
        if(inode_table[i].i_number==inode_no) //
        {
            return inode_table[i];
        }
    }

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

    // write the new loaded inode into the inode table
    bool tag=false;
    for(auto i=0; i<INODE_TABLE_SIZE;i++)
        if (inode_table[i].i_number==-1)
        {
            inode_table[i]=return_inode;
            tag=true;
            break;
        }
    if(!tag)
    {
        srand((unsigned int)time(NULL));
        
        int num_deprecated= rand()%INODE_TABLE_SIZE;

        inode_table[num_deprecated]=return_inode;
    }

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
        cur_dir_node=search(cur_dir_node, paths.at(i).data()); // the father directory of the target one

    if(cur_dir_node.i_flag & inode_flag::DIR_FILE != inode_flag::DIR_FILE)
    {
        std::cerr<<"[ERROR]can not create directory in a file.\n";
        return;
    }

    //TODO: add the judgement to the permissions


    if(cur_dir_node.i_size == (6+128*2 + 128*128*2)*BLOCK_SIZE)
    {
        std::cerr<<"[ERROR]the directory is full.\n";
        return ;
    }
    DirItem * dirlist = new DirItem [cur_dir_node.i_size/ DIR_ITEMS_SIZE];

    read_(cur_dir_node,(char*) dirlist, 0, cur_dir_node.i_size);

    for( auto i=0; i<cur_dir_node.i_size/DIR_ITEMS_SIZE; i++)
    {
        if( std::string(dirlist[i].name)== last_dir_name)
        {
            std::cerr<<"[ERROR]the directory "<<last_dir_name<<" is already existed.\n";
            return;
        }

    }

    delete [] dirlist;


// CREATE a new inode of newly created directory
    Inode new_inode = alloc_inode();

    new_inode.i_mode=IALLOC | DIR_FILE;

    new_inode.i_nlink=1;
    new_inode.i_uid=u_id;
    new_inode.i_gid=g_id;
    new_inode.i_size=0;

    new_inode.i_mtime= (unsigned int)(time(NULL));
    new_inode.i_atime= (unsigned int)(time(NULL));

    save_inode(new_inode);

// write the basic content of the newly created directory to a blk.
    DirItem new_dir_items[2];
    strcpy(new_dir_items[0].name, ".");
    new_dir_items[0].inode_no=new_inode.i_number;

    strcpy(new_dir_items[1].name,"..");
    new_dir_items[1].inode_no=cur_dir_node.i_number;

    write_(new_inode, (char*)new_dir_items,0, 2*DIR_ITEMS_SIZE);

    save_inode(new_inode);




// mend the inode of current directory  and write a new dir item into blk
    DirItem new_item_ ;
    strcpy(new_item_.name, last_dir_name.data());
    new_item_.inode_no=new_inode.i_number;


    //cur_dir_node.i_nlink++;
    write_(cur_dir_node,(char*)&new_item_ , cur_dir_node.i_size, DIR_ITEMS_SIZE);
    save_inode(cur_dir_node);

    return;
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

    // read the dir file
    bool tag=false;
    Inode return_inode;
    read_(cur_dir_inode, (char*)items, 0, sizeof(items));
    for( auto item: items)
    {
        if(strcmp(item.name,name)==0)
        {
            return_inode=load_inode(item.inode_no);
            tag=true;
            break;
        }

    }
    if(!tag)
        std::cerr<<"[ERROR]can not find file/dir "<<name<<'\n';

    return return_inode;
}


void FileSystem::read_(Inode & inode, char * buf, unsigned int start, unsigned int len)
{
    if(inode.i_size==0 || len==0)
        return;
    if( start + len > inode.i_size)
        len=inode.i_size-start; // do not read too must , just limited in this file


    int start_blk_num=start/BLOCK_SIZE;

    int c_len=0; // number of bytes which are already read 
    int start_ =0;
    int len_=0; // LENGTH read in one pass
    start_ = start%BLOCK_SIZE;// start_pos in one block

    bool tag=false;

    // start position is within the range of direct dir (6 blks)
    if(start_blk_num<6)
    {
        for(int i=start_blk_num;i<6;i++)
        {
            Buf * bp=this->br_mgr.Bread(0,inode.i_addr[i]);

            
            len_ =0; // length in a read pass

            if (len-c_len>= BLOCK_SIZE-start_)
                len_=BLOCK_SIZE-start_;
            else
                len_=len-c_len;


            io_move(bp->b_addr+start_,buf+c_len, len_  );

            c_len+=len_;
            start_=0;

            if(c_len==len)
            {
                tag=true;
                break;
            }

        }

        for(int i=6;i<8&& !tag;i++)
        {
            Buf * bp= br_mgr.Bread(0,inode.i_addr[i]);

            int _1st_index_blk [BLOCK_SIZE]; 
            io_move(bp->b_addr,(char*)_1st_index_blk,BLOCK_SIZE);

            for(int j=0 ;j<BLOCK_SIZE/sizeof(int); j++)
            {
                if(len-c_len>=BLOCK_SIZE-start_)
                    len_=BLOCK_SIZE-start_;
                else
                    len_=len-c_len;

                Buf* bp_=br_mgr.Bread(0,_1st_index_blk[j]);
                io_move(bp->b_addr+start_,buf+c_len,len_);

                c_len+=len_;
                start_=0;

                if(c_len=len)
                    break;

            }
        }

        for(int i=8;i<10 && !tag ;i++)
        {
            Buf* bp=br_mgr.Bread(0,inode.i_addr[i]);

            int _2nd_index_blk1[BLOCK_SIZE];
            io_move(bp->b_addr,(char*)_2nd_index_blk1, BLOCK_SIZE);

            for(int j=0;j<BLOCK_SIZE/sizeof(int);j++)
            {
                Buf * bp_=br_mgr.Bread(0,_2nd_index_blk1[j]);
                int _2nd_index_blk2[BLOCK_SIZE/sizeof(int)]; // that's the blk whose content are directories of real files

                io_move(bp_->b_addr,(char*)_2nd_index_blk2,BLOCK_SIZE);

                for(int k=0;k< BLOCK_SIZE/sizeof(int);k++)
                {
                    Buf* bp_content=br_mgr.Bread(0,_2nd_index_blk2[k]);
                    if(len-c_len>=BLOCK_SIZE-start_)
                        len_=BLOCK_SIZE-start_;
                    else
                        len_=len-c_len;                    
                    io_move(bp_content->b_addr+start_,buf+c_len,len_);

                    c_len+=len_;
                    start_=0;

                    if(c_len=len)
                        break;

                }
            }
        }


    }

    //// start position is within the range of 1st indirect dir (7- 128 *2 blks)
    else if(start_blk_num>= 6 && start_blk_num <6+ 128*2)
    {
        start_=start%BLOCK_SIZE;

        int start_dir_index= 6+ (start_blk_num-6)/128;
        for(int i=start_dir_index;i<8;i++)
        {
            Buf * bp= br_mgr.Bread(0,inode.i_addr[i]);

            int _1st_index_blk [BLOCK_SIZE]; 
            io_move(bp->b_addr,(char*)_1st_index_blk,BLOCK_SIZE);

            int start_dir_index1 = (i==start_dir_index)? (start_blk_num-6)%128 : 0;            
            for(int j=start_dir_index1 ;j<BLOCK_SIZE/sizeof(int); j++)
            {
                if(len-c_len>=BLOCK_SIZE-start_)
                    len_=BLOCK_SIZE-start_;
                else
                    len_=len-c_len;

                Buf* bp_=br_mgr.Bread(0,_1st_index_blk[j]);
                io_move(bp->b_addr+start_,buf+c_len,len_);

                c_len+=len_;
                start_=0;

                if(c_len=len)
                {
                    tag=true;
                    break;
                }

            }


        }

        for(int i=8;i<10 && !tag;i++)
        {
            Buf* bp=br_mgr.Bread(0,inode.i_addr[i]);

            int _2nd_index_blk1[BLOCK_SIZE];
            io_move(bp->b_addr,(char*)_2nd_index_blk1, BLOCK_SIZE);

            for(int j=0;j<BLOCK_SIZE/sizeof(int);j++)
            {
                Buf * bp_=br_mgr.Bread(0,_2nd_index_blk1[j]);
                int _2nd_index_blk2[BLOCK_SIZE]; // that's the blk whose content are directories of real files

                io_move(bp_->b_addr,(char*)_2nd_index_blk2,BLOCK_SIZE);

                for(int k=0;k< BLOCK_SIZE/sizeof(int);k++)
                {
                    Buf* bp_content=br_mgr.Bread(0,_2nd_index_blk2[k]);
                    if(len-c_len>=BLOCK_SIZE-start_)
                        len_=BLOCK_SIZE-start_;
                    else
                        len_=len-c_len;                    
                    io_move(bp_content->b_addr+start_,buf+c_len,len_);

                    c_len+=len_;
                    start_=0;

                    if(c_len=len)
                        break;

                }
            }
        }
    }

    // 6 direct dir items

    else if ( start_blk_num >= 6+128*2 && start_blk_num< 6+ 128 *2 + 128 * 128 *2)
    {
        start_=start%BLOCK_SIZE;

        int start_dir_index=8+ (start_blk_num- (6+128*2))/(128*128);

        for(int i=start_dir_index;i<10 && !tag;i++)
        {
            Buf* bp=br_mgr.Bread(0,inode.i_addr[i]);

            int _2nd_index_blk1[BLOCK_SIZE];
            io_move(bp->b_addr,(char*)_2nd_index_blk1, BLOCK_SIZE);

            int start_dir_index1=(i==start_dir_index1)? (start_blk_num-(6+128*2)-(i-start_dir_index1)*128*128)/128: 0; 

            for(int j=start_dir_index1;j<BLOCK_SIZE/sizeof(int);j++)
            {
                Buf * bp_=br_mgr.Bread(0,_2nd_index_blk1[j]);
                int _2nd_index_blk2[BLOCK_SIZE]; // that's the blk whose content are directories of real files

                io_move(bp_->b_addr,(char*)_2nd_index_blk2,BLOCK_SIZE);


                int start_dir_index2= (j==start_dir_index1 && i== start_dir_index)? (start_blk_num-(6+128*2)-(i-start_dir_index1)*128*128)%128:0;

                for(int k=start_dir_index2;k< BLOCK_SIZE/sizeof(int);k++)
                {
                    Buf* bp_content=br_mgr.Bread(0,_2nd_index_blk2[k]);
                    if(len-c_len>=BLOCK_SIZE-start_)
                        len_=BLOCK_SIZE-start_;
                    else
                        len_=len-c_len;                    
                    io_move(bp_content->b_addr+start_,buf+c_len,len_);

                    c_len+=len_;
                    start_=0;

                    if(c_len=len)
                        break;

                }
            }
        }

    }
    



    
    
}


void FileSystem:: write_(Inode &inode, char * buf, unsigned int start, unsigned int len)
{

    if(start>inode.i_size)
    {
        std::cerr<<"[ERROR]the start address is larger than the file.\n";
        return;
    }

    if(start+len> (6+128*2 + 2* 128 * 128)*BLOCK_SIZE )
    {
        len= (6+128*2 + 2* 128 * 128)*BLOCK_SIZE - start;
    }
    int start_blk_no= start/BLOCK_SIZE; // logically
    int cur_blk_no=inode.i_size/BLOCK_SIZE; //logically 
    int passed_blk_no=start_blk_no;// logical

    int cur_len=0;   // the bytes num we already written 
    int cur_start=0; //pointer in the buf
    bool tag=false;

    bool is_new_blk=false;
    int start_=start%BLOCK_SIZE; //pointer inside a blk


    if(start_blk_no< 6)
    {
        // handle the situation when 
        if(inode.i_size==0)
        {
            int first_blk_no=alloc_blk();
            inode.i_addr[0]=first_blk_no;
        }
        int start_dir_index= start_blk_no%6;

        for(int i=start_dir_index;i<6 && i<=cur_blk_no ;i++)
        {
            Buf* bp=br_mgr.Bread(0,inode.i_addr[i]);

            if(len-cur_len>BLOCK_SIZE-start_)
            {
                //if(i==start_dir_index)
                io_move(buf+cur_start, bp->b_addr+start_,BLOCK_SIZE-start_);
                cur_start+=BLOCK_SIZE-start_;
                cur_len+=BLOCK_SIZE-start_;
                br_mgr.Bdwrite(bp);
                start_=0;
            }
                
            else
            {
                io_move(buf+cur_start,bp->b_addr+start_,len-cur_len);
                cur_start+=len-cur_len;
                cur_len=len;
                br_mgr.Bdwrite(bp);
                start_=0;
            }

            passed_blk_no+=1;
            if(passed_blk_no>cur_blk_no)
                is_new_blk=true;

            if( cur_len==len)
            {
                tag=true;
                break;
            }

        }

        
        for(int i=cur_blk_no+1; i< 6 && !tag ; i++)
        {
            int new_blk_no= alloc_blk();
            inode.i_addr[i]=new_blk_no;

            Buf * bp= br_mgr.Bread(0, inode.i_addr[i]);
            if(len-cur_len>BLOCK_SIZE)
            {
                io_move(buf+cur_start, bp->b_addr,BLOCK_SIZE);
                cur_start+=BLOCK_SIZE;
                cur_len+=BLOCK_SIZE;
                br_mgr.Bdwrite(bp);
            }
            else
            {
                io_move(buf+cur_start,bp->b_addr,len-cur_len);
                cur_start+=len-cur_len;
                cur_len=len;
                br_mgr.Bdwrite(bp);
            }

            if( cur_len==len)
            {
                tag=true;
                break;
            }


        }

        // 1st index blk

        for(int i=6;i<8 && !tag; i++)
        {
            int _1st_index_blk[BLOCK_SIZE];

            if(is_new_blk)
            {
                int new_index_blk= alloc_blk();
                inode.i_addr[i]=new_index_blk;
            }
            Buf * _1st_dir_bp=  br_mgr.Bread(0,inode.i_addr[i]);

            io_move(_1st_dir_bp->b_addr,(char*)_1st_index_blk,BLOCK_SIZE);


            int _1st_index_start= 0;

            for(int j=_1st_index_start;j<BLOCK_SIZE/sizeof(int);j++)
            {
                if(is_new_blk)
                {
                    int new_index_blk= alloc_blk();
                    _1st_index_blk[j]=new_index_blk; //  do not forget to write it back
                }
                Buf * bp = br_mgr.Bread(0, _1st_index_blk[j]);

                if(len-cur_len>BLOCK_SIZE)
                {
                    io_move(buf+cur_start, bp->b_addr,BLOCK_SIZE);
                    cur_start+=BLOCK_SIZE;
                    cur_len+=BLOCK_SIZE;
                    br_mgr.Bdwrite(bp);
                }
                else
                {
                    io_move(buf+cur_start,bp->b_addr,len-cur_len);
                    cur_start+=len-cur_len;
                    cur_len=len;
                    br_mgr.Bdwrite(bp);
                }

                passed_blk_no+=1;
                if(passed_blk_no>cur_blk_no)
                    is_new_blk=true;

                if( cur_len==len)
                {
                    tag=true;
                    break;
                }
            }
        
        }
    }

    else if(start_blk_no< 6+2 * 128 && !tag)
    {
        int inode_dir_index= (start_blk_no-6)/128+6;

        for(int i=inode_dir_index;i<8;i++)
        {
            int _1st_index_blk[BLOCK_SIZE];

            if(is_new_blk)
            {
                int new_index_blk= alloc_blk();
                inode.i_addr[i]=new_index_blk;
            }
            Buf * _1st_dir_bp=  br_mgr.Bread(0,inode.i_addr[i]);

            io_move(_1st_dir_bp->b_addr,(char*)_1st_index_blk,BLOCK_SIZE);

            int _1st_index_start= (start_blk_no-6)%128;

            _1st_index_start= i==inode_dir_index? _1st_index_start:0;

            for(int j=_1st_index_start;j<BLOCK_SIZE/sizeof(int);j++)
            {
                if(is_new_blk)
                {
                    int new_index_blk= alloc_blk();
                    _1st_index_blk[j]=new_index_blk; //  do not forget to write it back
                }
                Buf * bp = br_mgr.Bread(0, _1st_index_blk[j]);

                if(len-cur_len>BLOCK_SIZE)
                {
                    io_move(buf+cur_start, bp->b_addr,BLOCK_SIZE);
                    cur_start+=BLOCK_SIZE;
                    cur_len+=BLOCK_SIZE;
                    br_mgr.Bdwrite(bp);
                }
                else
                {
                    io_move(buf+cur_start,bp->b_addr,len-cur_len);
                    cur_start+=len-cur_len;
                    cur_len=len;
                    br_mgr.Bdwrite(bp);
                }

                passed_blk_no+=1;
                if(passed_blk_no>cur_blk_no)
                    is_new_blk=true;

                if( cur_len==len)
                {
                    tag=true;
                    break;
                }
            }
        }
    }

    else 
    {
    }


    if(start+len>inode.i_size)
        inode.i_size=start+len;
    
}



/**** some functions used by users*****/
void FileSystem::list_(int inode_no)
{
    // note that the root dir is in the 0 inode
    int cur_dir_no=inode_no;
    Inode dir_inode=load_inode(cur_dir_no);

    if(dir_inode.i_flag && inode_flag::DIR_FILE == false)
        std::cerr<<"[ERROR]must list the files in the directory\n";

    int num_dir=dir_inode.i_size/DIR_ITEMS_SIZE;

    char * buf= new char [dir_inode.i_size];

    read_(dir_inode,buf,0,dir_inode.i_size);

    for(int i=0;i<num_dir;i++)
    {
        DirItem dir_item;
        memcpy((char*)&dir_item,buf+i*DIR_ITEMS_SIZE,DIR_ITEMS_SIZE);
        std::cout<<dir_item.name<<' ';
        if((i+1)%7==0)
            std::cout<<'\n';
    }
}

void FileSystem::list(std::string  route)
{
    std::vector<std::string> paths=split(route,'/');
    std::string last_dir_name=paths.at(paths.size()-1);


    Inode cur_dir_node=load_inode(0);

    for(unsigned int i=0;i<paths.size();i++)
        cur_dir_node=search(cur_dir_node, paths.at(i).data()); // the father directory of the target one

    list_(cur_dir_node.i_number);
}



FileSystem::FileSystem(/* args */)
{
}

FileSystem::~FileSystem()
{

}