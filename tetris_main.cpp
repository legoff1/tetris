#include "board.hpp"

int main(int argc, char *argv[]){

    Board game_board = Board();
    
    int exit_status = game_board.display_game();

    return exit_status;
}