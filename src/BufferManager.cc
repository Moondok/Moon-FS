#include <BufferManager.h>
#include <DeviceManager.h>

void  BufferManager:: Bwrite( Buf * bp)
{
    
}

Buf* BufferManager:: Bread(int dev_no,int blk_no)
{
    Buf * bp=get_blk(dev_no,blk_no);

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

    for(auto bp=devtab->b_forw;bp!=nullptr;bp=bp->b_back)
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
            break;
        }

    }

    if(return_blk!=nullptr)
        return return_blk;

    // search the bfreelist
    for(auto bp=bFreeList.av_forw;bp!=nullptr;bp=bp->av_forw)
    {
        if(bp->b_flags&BufFlag::B_DELWRI)
        {
            bp->av_back->av_back=bp->av_back;
            bp->av_forw->av_back=bp->av_back;

            bp->b_flags|=BufFlag::B_BUSY;
            //LINK this buffer blk to the IO list
            this->get_device_manager()->get_blk_device()->Strategy(bp);

            this->Bwrite(bp);

            // DELETE the signal of delaying writ
            bp->b_flags&=(!BufFlag::B_DELWRI);

            // after writing, we move the buffer from the i/o list
            auto d_tab=this->get_device_manager()->get_blk_device()->get_devtab();
            d_tab->d_actl=d_tab->d_actl->av_back;

            this->Brelse(bp);
        

        }
        else // we get it 
        {
            return_blk=bp;
            bp->av_back->av_back=bp->av_back;
            bp->av_forw->av_back=bp->av_back;

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

    
}

BufferManager::BufferManager()
{
    m_DeviceManager=new DeviceManager();
}

BufferManager ::~BufferManager()
{
    if(m_DeviceManager!=nullptr)
        delete m_DeviceManager;
}
