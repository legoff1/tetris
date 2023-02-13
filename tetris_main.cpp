#include "tetris.hpp"

int main(int argc, char *argv[]){

    Board game_board = Board();
    
    int exit_status = game_board.create_window();

    return exit_status;
}