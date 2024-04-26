#include <iostream>
#include <FileSystem.h>

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
    }






    return 0;
}