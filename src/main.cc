#include <iostream>
#include <FileSystem.h>
#include<File.h>

int main()
{
    //catch the command arguments from the client
    /****
     * 
    */

   FileSystem fs;

   fs.initialize();

    // 
    int commands[6];


    if(commands[0]==1)
    {
        fs.format();
    }

    File * ptr;
    std::string command, param;
    while(true)
    {
        std::cin>>command;
        if(command=="exit")
            break;

        else if(command=="mkdir")
        {
            std::cin>>param;
            fs.create_dir(param.data(),0,0,0);
        }

        else if(command=="ls")
        {
            std::cin>>param;
            fs.list(param);
        }
        else if(command=="touch")
        {
            std::cin>>param;
            fs.create_file(param.data(),0,0,0);
        }
        else if(command=="open")
        {
            std::cin>> param;

            int mode;
            while(std::cin)
            {
                char c;
                std::cin>>c;
                if(c=='r')
                    mode|=File::FileFlags::FREAD;
                else if(c=='w')
                    mode|=File::FileFlags::FWRITE;
            }

            ptr = fs.open_file(param.data(),0,0,0,mode);
        }
        else if(command=="read")
        {
            std::cin>> param;

            std::cout<< "please input number of bytes you want to read:\n";

            int num;
            std::cin>>num;

            char *buf =new char[num+1];
            buf[num]='\0';

            fs.read_( *ptr->f_inode,buf, ptr->f_offset, num);

            std::cout<<"the "<<num<<" bytes from the position "<<ptr->f_offset<<" are :\n";
            std::cout<<buf<<'\n';

            delete [] buf;
        }
        else if(command=="write")
        {
            std::cin>> param;

            std::cout<< "please input number of bytes you want to read:\n";

            int num;
            std::cin>>num;

            char *buf =new char[num+1];
            buf[num]='\0';
            std::cin>>buf;

            fs.write_( *ptr->f_inode,buf, ptr->f_offset, num);

            std::cout<<"the "<<num<<" bytes from the position "<<ptr->f_offset<<" are :\n";
            std::cout<<buf<<'\n';

            delete [] buf;
        }

    }






    return 0;
}