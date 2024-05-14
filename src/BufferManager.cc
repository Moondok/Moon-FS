#include <BufferManager.h>
#include <DeviceManager.h>

void  BufferManager:: Bwrite( Buf * bp)
{
    std::fstream fout; 
    fout.open(DISK_FILE_NAME,  std::ios::in|std::ios::out| std:: ios:: binary);

    if(fout.is_open()==false)
        std::cerr<<"[ERROR]fail to open the disk\n";

    // save

    // fout.seekp(std::streampos(bp->b_blk_no+1)*std::streampos(BUFFER_SIZE),std::ios::beg);
    // char * buf=new char [DISK_SIZE-(bp->b_blk_no+1)*BUFFER_SIZE];
    // fout.read(buf,DISK_SIZE-(bp->b_blk_no+1)*BUFFER_SIZE);


    fout.seekp(std::streampos(bp->b_blk_no)*std::streampos(BUFFER_SIZE),std::ios::beg);
    fout.write((const char*)bp->b_addr,BUFFER_SIZE);
    //fout.write(buf,DISK_SIZE-(bp->b_blk_no+1)*BUFFER_SIZE);
    

    fout.close();

    //delete [] buf;

    // DELETE the signal of delaying writ
    bp->b_flags&=(!BufFlag::B_DELWRI);

    // after writing, we move the buffer from the i/o list
    auto d_tab=this->get_device_manager()->get_blk_device()->get_devtab();


    d_tab->d_actl=d_tab->d_actl->av_back;
    if(d_tab->d_actl==nullptr)
        d_tab->d_actf=nullptr;
    else
    {
        d_tab->d_actl->av_forw=nullptr;
    }

    this->Brelse(bp);

}

void BufferManager::Bdwrite(Buf* bp)
{
    bp->b_flags|=BufFlag::B_DELWRI;
}

Buf* BufferManager:: Bread(int dev_no,int blk_no)
{
    Buf * bp=get_blk(dev_no,blk_no);

    //std::cout<<(bp->b_flags & BufFlag::B_DONE)<<'\n';

    if((bp->b_flags & BufFlag::B_DONE ) == BufFlag::B_DONE)
    {
        //return bp;
        // note that in this branch , the block is not unlinked!
        reorder_buf(bp);
        
    }
    else
    {
        this->get_device_manager()->get_blk_device()->Strategy(bp);
        std::fstream fin;
        fin.open(DISK_FILE_NAME,std::ios::in|std::ios::binary);
        if(fin.is_open()==false)
            std::cerr<<"[ERROR]fail to open the disk\n";
        
        fin.seekg(std::streampos(blk_no)*std::streampos(BUFFER_SIZE),std::ios::beg);
        fin.read(bp->b_addr,BUFFER_SIZE);

        fin.close();

        auto d_tab=this->get_device_manager()->get_blk_device()->get_devtab();
        d_tab->d_actl=d_tab->d_actl->av_back;
        if(d_tab->d_actl==nullptr)
        d_tab->d_actf=nullptr;
        else
        {
            d_tab->d_actl->av_forw=nullptr;
        }

        this->Brelse(bp);
        //return bp;


    }

    return bp;


}

void BufferManager::Brelse(Buf* bp)
{
    bp->b_flags=BufFlag::B_DONE;



    Buf* last =bFreeList.av_back;

    Devtab * dtab=this->get_device_manager()->get_blk_device()->get_devtab();

    last->av_forw=bp;
    bp->av_back=last;
    bp->av_forw=nullptr;
    bFreeList.av_back=bp;
}

Buf* BufferManager ::get_blk(int dev_no,int blk_no)
{
    Buf * return_blk=nullptr;

    Devtab * devtab=this->get_device_manager()->get_blk_device()->get_devtab();

    for(auto bp=devtab->b_forw;bp!=nullptr;bp=bp->b_forw)
    {
        if(bp->b_dev!=dev_no || bp->b_blk_no!=blk_no)
            continue;
        
        //here we find the correct data blk, the we check whether it is used  by other users
        if(bp->b_flags & BufFlag::B_BUSY)
        {
            bp->b_flags|= BufFlag::B_WANTED;
            //TODO: add the case when other user is using this blk
        }

        if(bp->b_dev==dev_no && bp->b_blk_no==blk_no)
        {
            return_blk=bp;



            //for LRU:
            //for each block whose unused time less than target, the unused time +1
            for (auto bp_ = devtab->b_forw; bp_!=nullptr; bp_=bp_->b_forw)
            {
                if(bp_->unused_time<bp->unused_time)
                    bp_->unused_time++;
            }

            return_blk->unused_time=0;
            break;
        }

    }

    if(return_blk!=nullptr)
        return return_blk;

    // search the bfreelist
    for(auto bp=bFreeList.av_forw;bp!=nullptr;bp=bFreeList.av_forw)
    {
        if(bp->b_flags&BufFlag::B_DELWRI)
        {
            not_avaible(bp);

            bp->b_flags|=BufFlag::B_BUSY;
            //LINK this buffer blk to the IO list
            this->get_device_manager()->get_blk_device()->Strategy(bp);

            this->Bwrite(bp);
        }
        else // we get it 
        {
            return_blk=bp;
            bp->b_blk_no=blk_no;
            bp->b_dev=dev_no;


            //for LRU:
            for (auto bp_ = devtab->b_forw; bp_!=nullptr; bp_=bp_->b_forw)
            
                //if(bp_->unused_time<bp->unused_time)
                bp_->unused_time++;
            

            return_blk->unused_time=0;


            
            not_avaible(bp);
            bp->b_flags=BufFlag::B_BUSY; //lock it
            

            break;
        }
    }
    return return_blk;

}


DeviceManager * BufferManager::get_device_manager()
{
    return m_DeviceManager;
}

void BufferManager::not_avaible(Buf* bp)
{
    bp->av_back->av_forw=bp->av_forw;
    if(bp->av_forw!=nullptr)
        bp->av_forw->av_back = bp->av_back;

    if(bFreeList.av_back==bp)
        bFreeList.av_back=bp->av_back;

    
}

void BufferManager:: initialize()
{
    bFreeList.b_blk_no=-1;
    bFreeList.id=-1;
    bFreeList.b_addr=nullptr;
    bFreeList.b_flags=B_NONE;

    bFreeList.av_forw=&m_Buf[0];

    bFreeList.av_back=&m_Buf[NUM_BUFFER-1];

    for(int i=0;i<NUM_BUFFER;i++)
    {
        m_Buf[i].b_blk_no=-1;
        m_Buf[i].id=i;
        m_Buf[i].b_addr=Buffer[i];
        m_Buf[i].b_flags=BufFlag::B_NONE;

        if(i==0)
        {
            m_Buf[i].b_forw=&m_Buf[i+1];
            m_Buf[i].b_back=nullptr;
            m_Buf[i].av_forw=&m_Buf[i+1];
            m_Buf[i].av_back=&bFreeList;
        }
        else if(i!=NUM_BUFFER-1)
        {
            m_Buf[i].b_forw=&m_Buf[i+1];
            m_Buf[i].b_back=&m_Buf[i-1];
            m_Buf[i].av_forw=&m_Buf[i+1];
            m_Buf[i].av_back=&m_Buf[i-1];
        }
        else
        {
            m_Buf[i].b_back=&m_Buf[i-1];
            m_Buf[i].av_back=&m_Buf[i-1];
            m_Buf[i].av_forw=nullptr;
            m_Buf[i].b_forw=nullptr;
        }

    }

    auto disk_tab=this->get_device_manager()->get_blk_device()->get_devtab();

    disk_tab->b_forw=&m_Buf[0];

    disk_tab->b_back=nullptr;

    disk_tab->d_actf=nullptr;
    disk_tab->d_actl=nullptr;

}

// note that this function only be invoked when some block is already existed in buffers
void BufferManager::reorder_buf(Buf* bp)
{
    // to achieve computationally cheap, we do not perform sorting algorithm,
    // we just put the target the buffer block to the end of buffer link, the effect is the same with sorting
    not_avaible(bp);

    Buf* last =bFreeList.av_back;
    last->av_forw=bp;
    bp->av_back=last;
    bp->av_forw=nullptr;
    bFreeList.av_back=bp;
    
}

BufferManager::BufferManager()
{
    m_DeviceManager=new DeviceManager();

    initialize();
}

BufferManager ::~BufferManager()
{
    int cnt=0;
    for(Buf* bp=bFreeList.av_forw; cnt<15;cnt++)
    {
        not_avaible(bp);
        get_device_manager()->get_blk_device()->Strategy(bp);
        Bwrite(bp);

        // the newly written block will be the last position of queue
        bp=bFreeList.av_forw;
    }
    if(m_DeviceManager!=nullptr)
        delete m_DeviceManager;
}
