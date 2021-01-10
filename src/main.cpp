#include "gofl_board.hpp"


int main(){
    GoflBoard life("example.txt");
    if(!life.Empty())
        life.Run();
    return 0;
}