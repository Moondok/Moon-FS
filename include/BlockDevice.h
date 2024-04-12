# ifndef __BLOCK_DEVICE_H__
# define __BLOCK_DEVICE_H__

# include <Buf.h>



class Devtab
{
public:
    int d_active; 
    int d_eerrcnt;
    Buf * b_forw;
    Buf * b_back;
    Buf * d_actf ;// IO request queue
    Buf * d_actl; 


};

class BlockDevice
{
public: 
    int Strategy(Buf*bp);
    Devtab * get_devtab();

    BlockDevice();
    ~BlockDevice();

private:
    Devtab * d_tab ;  // only one device table for we have one disk only

};


# endif