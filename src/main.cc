#include <iostream>
#include <FileSystem.h>
#include <File.h>

int main()
{
    //catch the command arguments from the client
    /****/


    // char *s =new char[81];

    // char * b=new char[801];
    // for(int i=0;i<80;i++)
    // {
    //     s[i]='1';
    //     for(int j=2;j<10;j++)
    //         b[10*i+j]=j+'0';
    //     b[10*i]=i/10+'0';
    //     b[10*i+1]=i%10+'0';
    // }
    // b[800]='\0';
    // s[80]='\0';
    // std::cout<<b;
    // std::cout<<'\n'<<s;

    // delete[] b;
    // delete [] s;

    


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
        std::cout<<'\n';
        std::cout<<"please input user name:";
        std::string u_name,u_password;
        std::cin>>u_name;

        std::cout<<"\nplease input password:";
        std::cin>> u_password;

        int re= fs.login_(u_name, u_password);

        if(re==0)
        {
            std::cout<<"login success.\n";
            break;
        }

        else 
            std::cout<<"the user name or password is wrong, please input again.\n";
    }

    auto usr_cur_dir_names_= fs.get_usr_cur_names();
    auto usr_cur_dir_inode_no= fs.get_usr_cur_dir_no();


    while(true)
    {
        std::cout<<"Moon-FS:";
        for(auto dir_item : usr_cur_dir_names_)
            std::cout<<"/"<<dir_item;
        std::cout<<"$ ";

        std::cin>>command;
        if(command=="exit")
            break;

        else if(command=="mkdir")
        {
            std::cin>>param;
            fs.create_dir(param.data(),0,0,usr_cur_dir_inode_no);
        }

        else if(command=="ls")
        {
            std::cin>>param;
            fs.list(param);
        }
        else if(command=="touch")
        {
            std::cin>>param;
            fs.create_file(param.data(),0,0,usr_cur_dir_inode_no);
        }
        else if(command=="open")
        {
            std::cin>> param;

            int mode;
            
            char c;
            std::cin>>c;
            if(c=='r')
                mode|=File::FileFlags::FREAD;
            else if(c=='w')
                mode|=File::FileFlags::FWRITE;

            std::cin>>c;
            if(c=='r')
                mode|=File::FileFlags::FREAD;
            else if(c=='w')
                mode|=File::FileFlags::FWRITE;
            

            ptr = fs.open_file(param.data(),0,0,usr_cur_dir_inode_no,mode);
        }
        else if(command=="read")
        {
            std::cin>> param;

            std::cout<< "please input number of bytes you want to read:\n";

            int num;
            std::cin>>num;

            char *buf =new char[num+1];
            buf[num]='\0';

            int len=fs.read_( *ptr->f_inode,buf, ptr->f_offset, num);
            ptr->f_offset+=len;

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

            int len=fs.write_( *ptr->f_inode,buf, ptr->f_offset, num);

            ptr->f_offset+=len;
            std::cout<<"the "<<num<<" bytes from the position "<<ptr->f_offset<<" are :\n";
            std::cout<<buf<<'\n';

            delete [] buf;
        }
        else if(command=="seekp")
        {
            int offset;
            std::cin>>offset;
            int base;
            std::cin>>base;
            fs.seekp(ptr,offset,base);
        }

        else if(command=="cd")
        {
            std::cin>>param;
            fs.change_directory(param.data(),0,0,usr_cur_dir_inode_no);
            usr_cur_dir_names_= fs.get_usr_cur_names();
            usr_cur_dir_inode_no= fs.get_usr_cur_dir_no();
        }

        else if(command=="stat")
        {
            std::cin>>param;
            fs.check_status(param.data(),0,0,usr_cur_dir_inode_no);
        }

        else if (command=="rm")
        {
            std::cin>>param;
            if(param=="-r")
            {
                std::string dir_name;
                std::cin>>dir_name;
                fs.delete_dir(dir_name.data(),0,0,usr_cur_dir_inode_no);
            }
            else
                fs.delete_file(param.data(),0,0,usr_cur_dir_inode_no);
                
        }

        else if(command=="test")
        {
            std::cin>>param;
            if(param=="image")
            {
                fs.create_file("home/photos/disparity.png",0,0,0);
                ptr = fs.open_file("home/photos/disparity.png",0,0,0,File::FileFlags::FWRITE|File::FileFlags::FREAD);
                int num=54810;
                char *buf =new char[num+1];
                buf[num]='\0';

                std::fstream fin;
                fin.open("disparity.png",std::ios::in|std::ios::binary);
                fin.read(buf,num);
                fin.close();
                int len=fs.write_( *ptr->f_inode,buf, ptr->f_offset, num);
                ptr->f_offset+=len;
                delete [] buf;
                fs.seekp(ptr,0,0);
                buf =new char[num+1];
                buf[num]='\0';
                len=fs.read_( *ptr->f_inode,buf, ptr->f_offset, num);
                ptr->f_offset+=len;
                std::cout<<"the "<<num<<" bytes from the position "<<ptr->f_offset<<" are :\n";
                std::fstream fo;
                fo.open("disparity_test.png",std::ios::out|std::ios::binary);
                fo.write(buf,num);
                fo.close();
                delete [] buf;

                fs.close_file(ptr);
            }
            else if(param=="doc")
            {
                fs.create_file("home/reports/report.pdf",0,0,0);
                ptr = fs.open_file("home/reports/report.pdf",0,0,0,File::FileFlags::FWRITE|File::FileFlags::FREAD);
                int num=6446724;
                char *buf =new char[num+1];
                buf[num]='\0';

                std::fstream fin;
                fin.open("report.pdf",std::ios::in|std::ios::binary);
                fin.read(buf,num);
                fin.close();
                int len=fs.write_( *ptr->f_inode,buf, ptr->f_offset, num);
                ptr->f_offset+=len;
                delete [] buf;
                fs.seekp(ptr,0,0);
                buf =new char[num+1];
                buf[num]='\0';
                len=fs.read_( *ptr->f_inode,buf, ptr->f_offset, num);
                ptr->f_offset+=len;
                std::cout<<"the "<<num<<" bytes from the position "<<ptr->f_offset<<" are :\n";
                std::fstream fo;
                fo.open("report_test.pdf",std::ios::out|std::ios::binary);
                fo.write(buf,num);
                fo.close();
                delete [] buf;

            }
            else if(param=="readme")
            {
                fs.create_file("home/texts/readme.txt",0,0,0);
                ptr = fs.open_file("home/texts/readme.txt",0,0,0,File::FileFlags::FWRITE|File::FileFlags::FREAD);
                int num=454;
                char *buf =new char[num+1];
                buf[num]='\0';

                std::fstream fin;
                fin.open("readme.txt",std::ios::in|std::ios::binary);
                fin.read(buf,num);
                fin.close();
                int len=fs.write_( *ptr->f_inode,buf, ptr->f_offset, num);
                ptr->f_offset+=len;
                delete [] buf;
                fs.seekp(ptr,0,0);
                buf =new char[num+1];
                buf[num]='\0';
                len=fs.read_( *ptr->f_inode,buf, ptr->f_offset, num);
                ptr->f_offset+=len;
                std::cout<<"the "<<num<<" bytes from the position "<<ptr->f_offset<<" are :\n";
                std::fstream fo;
                fo.open("readme_test.txt",std::ios::out|std::ios::binary);
                fo.write(buf,num);
                fo.close();
                delete [] buf;

                fs.close_file(ptr);
            }

        }

        else
        {
            std::cerr<<"Command \""<<command<<"\" is not found.\n";
            std::cin.clear();
        }

        std::cout<<"empty block number is : "<<fs.superblock.s_block_free_num<<" \n";
        std::cout<<"empty inode number is : "<<fs.superblock.s_inode_free_num<<" \n";

    }






    return 0;
}