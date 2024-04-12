#include <BlockDevice.h>

int BlockDevice:: Strategy (Buf* bp)
{
    // link the buffer in the 
    bp->av_forw=nullptr;

    if(this->d_tab->d_actf==nullptr)
        this->d_tab->d_actf=bp;
    else 
        this->d_tab->d_actl->av_forw=bp; // note that av_forw points to the following element in linked list

    this->d_tab->d_actl=bp;


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