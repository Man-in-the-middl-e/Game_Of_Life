#include "gofl_board.hpp"
#include <iostream>
#include<string.h>
void PrintHelp()
{
    std::cout << "Usage: ./life -f file_name" << std::endl;
    std::cout << "Try to run: ./life -f  ../exmaple.txt" << std::endl;
    ;
}
int main(int argc, char *argv[])
{

    if (argc <= 2)
    {
        PrintHelp();
        return -1;
    }
    else
    {
        if (strncmp(argv[1], "-f", 2) != 0)
        {
            PrintHelp();
            return -1;
        }
        const std::string file_name = std::string(argv[2]);
        GoflBoard life(file_name);
        if (!life.Empty())
            life.Run();
    }
}