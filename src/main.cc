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






    return 0;
}