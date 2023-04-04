//
// Created by Andre Werneck on 13/12/2022.
//

#include "tetris.hpp"
#include "board.hpp"

// =============================================== CLASS TETRINO STATE -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================

const uint8_t Tetrino_state::get_position(const Tetrino *tetrino, int32_t row, int32_t col, int32_t rot){
    // int32_t dimension = tetrino->get_form_shape();
    int32_t side = tetrino->side;
    // const uint8_t* tetrino_data = tetrino->get_data();
    switch (rot)
    {
    case 0:
        return tetrino->data[row * side + col];

    // making the projection of the coordenate system to the rotated one (finding the new coordenates of the original rows and cols)
    case 90:
        return tetrino->data[(side - col - 1) * side + row];
    case 180:
        return tetrino->data[(side - row - 1) * side + (side - col - 1)];
    case 270: 
        return tetrino->data[col * side + (side - row - 1)];
    }
    return 0;
}

// =============================================== CLASS TETRIS -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================


float Tetris::get_next_drop_time(){

    if(level >= 29){
        level = 29;
    }

    return FRAMES_PER_LEVEL[level] * SECONDS_PER_FRAME;
}

int32_t Tetris::compute_points(){
    // compute the points according to the tetris nintendo DS version and the docs of Monsuez as well
    switch (filled_line_count)
    {
    case 1: return 40 * (level+1);
        break;
    case 2: return 100 * (level+1); 
        break;
    case 3: return 300 * (level+1);
        break;
    case 4: return 1200 * (level+1);
    }
    return 0;
}

int32_t Tetris::max(int32_t x, int32_t y){
    return x > y ? x : y;
}

int32_t Tetris::min(int32_t x, int32_t y){
    return x < y ? x : y;
}

void Tetris::spawn_tetrino(){
    uint8_t aux = 0;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist7(0,6); // generating a random index to pick up the new tetrino randomly without any bias
    uint8_t randidx = dist7(rng);
    uint8_t randidx_next = dist7(rng);

    // std::cout<<int(randidx)+1<<std::endl;
    // std::cout<<int(randidx_next)+1<<std::endl;

    if(first_piece){

        piece = {};
        next_piece = {};
        piece.index = randidx;
        next_piece.index = randidx_next;
        first_piece = false;

    }
    else{
        aux = next_piece.index;
        next_piece.index = randidx_next;
        piece.index = aux;
    }
        
    piece.offset_row = 0; 
    piece.offset_col = WIDTH/2;
    next_tetrino_drop_time = time + get_next_drop_time();

}

void Tetris::handle_hold_state()
{   // hold phase just happens in play phase
    
    uint8_t aux_index;

    if(hold_piece==false){
        // there wasn`t a piece holded
        //so we hold the current piece and we spawn a new one as usual
        holded_piece.index = piece.index;
        spawn_tetrino();
        hold_piece=true;
        hold_valid=false;
    }
    else if(hold_valid){
        // if there was already a piece holded we just change the piece
        aux_index = piece.index;
        piece.index = holded_piece.index;
        holded_piece.index = aux_index;
        piece.offset_row = 0; //setting the row to the first one
        piece.offset_col = WIDTH/2;
        next_tetrino_drop_time = time + get_next_drop_time();
        hold_valid = false;
    }

    phase = TETRIS_GAME_PLAY; //go back to the play phase
    
}

int32_t Tetris::check_lines_to_next_level(){
    // computing the limits to go up on levels according to the tetris Nintendo DS
    int32_t threshold_to_go_up_level_one = min((start_level * 10 + 10),max(100,(start_level*10 - 50)));
    if(level == start_level){
        return threshold_to_go_up_level_one;
    }
    else{
        int32_t difference = level - start_level;
        return threshold_to_go_up_level_one + difference*10; // we need to fill 10 lines plus in each level to go up to the next level 
    }
}

void Tetris::update_game_over_state(Keyboard *input){
    if(input->dspace > 0){
        phase = TETRIS_GAME_START; // if there is gameover, we can press the space bar to restart the game without exiting
    }
}

void Tetris::update_game_start_state(Keyboard *input)
{
    if(input->dup>0){
        ++start_level; // press up to raise the starting level
    }

    
    if(input->ddown>0){
        --start_level; // press down to low the starting level
    }

    if(input->dspace>0){
        
        //clears the board
        memset(board,0,(WIDTH*HEIGHT));
        // set or reset the params of the game
        level = start_level;
        line_count = 0;
        points = 0;
        first_piece = true;
        spawn_tetrino();
        
        phase = TETRIS_GAME_PLAY; // press space bar to begin the game at the level selected
    }
}
