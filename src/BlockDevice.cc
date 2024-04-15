#include <BlockDevice.h>

int BlockDevice:: Strategy (Buf* bp)
{
    // link the buffer in the 
    bp->av_forw=nullptr;

    if(this->d_tab->d_actf==nullptr)
    {
        this->d_tab->d_actf=this->d_tab->d_actl=bp;
        bp->av_back=nullptr;
    }
    else 
    {
        bp->av_back=this->d_tab->d_actl;
        bp->av_back->av_forw=bp;
        this->d_tab->d_actl=bp; // note that av_forw points to the following element in linked list
    }

    

    this->d_tab->d_actl=bp;

    return 0;


}

Devtab* BlockDevice::get_devtab()
{
    return d_tab;
}

BlockDevice::BlockDevice()
{
    d_tab=new Devtab();
}

BlockDevice::~BlockDevice()
{
    if(d_tab!=nullptr)
        delete d_tab;
}