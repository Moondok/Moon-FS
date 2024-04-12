#include <BufferManager.h>
#include <DeviceManager.h>

void  BufferManager:: Bwrite( Buf * bp)
{
    
}

Buf* BufferManager:: Bread(int dev_no,int blk_no)
{
    Buf * bp=get_blk(dev_no,blk_no);

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

    }

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
