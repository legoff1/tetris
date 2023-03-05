//
// Created by Andre Werneck on 13/12/2022.
//

#ifndef TETRIS_TETRIS_H
#define TETRIS_TETRIS_H

#include <stdint.h>
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include "colors.hpp"
#include "tetrino.hpp"
#include "frames_per_level.hpp"
#include <assert.h>

#define WIDTH 10
#define HEIGHT 22 // 2 lines invisible to pop up new peaces without show to the user
#define GRID_SIZE 30

// =============================================== CLASS TETRINO STATE -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================


class Tetrino_state
{
public:
    uint8_t index; // index to inform which tetrino is it (S,T,O,I, etc)
    int32_t offset_row; // row and column to indetify where the piece currently is in the gme field (board)
    int32_t offset_col;
    int32_t rotation;
    // explicit Tetrino_state(): index(0), offset_row(0), offset_col(0),rotation(0){}
    const uint8_t get_position(const Tetrino *tetrino,int32_t row, int32_t col,int32_t rot); // find the position of a tetrino even if it is rotated or not
};

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

class Tetris
{
    public:
    uint8_t board[WIDTH * HEIGHT];
    // uint8_t lines[HEIGHT];
    // int32_t pending_line_count;
    
    Tetrino_state piece;

    int32_t level;

    float time;// absolute time of the game
    float next_tetrino_drop_time; // time to drop the next tetrino

    float get_next_drop_time();
};

float Tetris::get_next_drop_time(){

    if(level > 29){
        level = 29;
    }

    return FRAMES_PER_LEVEL[level] * SECONDS_PER_FRAME;
}


// =============================================== CLASS KEYBOARD -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================
class Keyboard // take the inputs of the keyboard
{
    public:
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;

    uint8_t a;
    
    int8_t dleft;
    int8_t dright;
    int8_t dup;
    int8_t ddown;
    int8_t da;
};

// =============================================== CLASS BOARD -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================

class Board{
private:
    const int32_t width;
    const int32_t height;
public:
    explicit Board(): width(WIDTH), height(HEIGHT){} //setting the constructor

    const int32_t get_width(); // get the width of the board
    const int32_t get_height(); // get the height of the board
    // uint8_t* set_board(); // creates a board with the right dimensions of the game - projection a 2d array in a same dimension 1d array
    uint8_t get_boardmatrix(uint8_t* brd,int32_t row, int32_t col) const; // get the value of an index of the board from a 2d matrix
    void set_boardmatrix(uint8_t* board,int32_t row, int32_t col, uint8_t value); // set the value of an index of the board in a 2d matrix
    int display_game(); // create and displays a window with SDL2
    void fill_board_rect(SDL_Renderer *renderer, int32_t coord_x, int32_t coord_y, int32_t w, int32_t h, Color color); // just filling the board with the rect method of SDL to get the tetrinos
    void draw_onboard(SDL_Renderer *renderer, int32_t row, int32_t col, uint8_t value, int32_t delta_x, int32_t delta_y); // now we are drawing the rect (cells/pieces) filled above
    void draw_tetrino(SDL_Renderer *renderer, Tetrino_state *t_state, int32_t delta_x, int32_t delta_y); // drawing and rendering the JUST tetrinos using the draw_onboard method
    void render_game(Tetris *tetris_game, SDL_Renderer *renderer); // uses the functions above to renderr the game itself with the pieces and other features
    bool check_board_limits(uint8_t* brd,Tetrino_state *tetrino_state); // set the limits of the board and check if the piece is at the bounderies or in colision with another piece.
    void update_tetrino_state(Tetris *game, Keyboard *input); // update positions according the the inputs on keyboard
    void soft_drop(Tetris *game); // do the soft drop of a piece in the board. If the piece reaches the board limits, we merge it to the board.
    void merge_tetrino_into_board(Tetris *game); // merges the tetrino into board when it reaches board`s limit.
    void spawn_tetrino(Tetris *game); // spawn a new tetrino after the last one reaches the limit of the board.
    void draw_board(SDL_Renderer *renderer, uint8_t *brd, int32_t offset_x, int32_t offset_y); // draw the game board WITHIN the tetrinos present on it
};

const int32_t Board::get_width(){
    return width;
}

const int32_t Board::get_height(){
    return height; 
}

uint8_t Board::get_boardmatrix(uint8_t* brd,int32_t row, int32_t col) const{ 
    int32_t index = row * width + col;
    return brd[index]; // tells us if there is or not any cell placed at the index on the board! 
}

void Board::set_boardmatrix(uint8_t* board,int32_t row, int32_t col, uint8_t value){
    int32_t index = row * width + col;
    board[index] = value;
}

int Board::display_game(){

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        "Tetris",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width*40,
        height*40,
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC
    );

    Tetris game = {};
    Keyboard input = {};

    spawn_tetrino(&game);

    game.piece.index=2;


    if(NULL == window){
        std::cout << "Error Detected" << std::endl;
        return 1;
    }
   
    bool quit = false;
    while(!quit){
        float time = SDL_GetTicks()/1000.0f;
        game.time = time;
        SDL_Event windowEvent;
        if (SDL_PollEvent(&windowEvent)){
            if (SDL_QUIT==windowEvent.type){
                std::cout << "Quit Screen" << std::endl;
                quit=true;
            }
        }

        int32_t num_keys;
        const uint8_t *key_states = SDL_GetKeyboardState(&num_keys);

        if(key_states[SDL_SCANCODE_ESCAPE]){
            quit = true;
        }

        Keyboard old_input = input;

        input.left = key_states[SDL_SCANCODE_LEFT];
        input.right = key_states[SDL_SCANCODE_RIGHT];
        input.up = key_states[SDL_SCANCODE_UP];
        input.down = key_states[SDL_SCANCODE_DOWN];

        input.dleft = input.left - old_input.left;
        input.dright = input.right - old_input.right;
        input.dup = input.up - old_input.up;
        input.ddown = input.down - old_input.down;
        
        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_RenderClear(renderer);
        update_tetrino_state(&game,&input);
        render_game(&game,renderer);
        // render_game(&tet1,renderer);
        SDL_RenderPresent(renderer);
        // game.piece.offset_row+=1;
        // tet.rotation = (tet.rotation + 90) % 360;
        // SDL_Delay(500);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void Board::fill_board_rect(SDL_Renderer *renderer, int32_t coord_x, int32_t coord_y, int32_t w, int32_t h, Color color){
    SDL_Rect board_rect;
    
    board_rect.x = coord_x; // the x location of the rectangle's upper left corner
    board_rect.y = coord_y; // the y location of the rectangle's upper left corner
    board_rect.w = w; // the width of the rectangle
    board_rect.h = h; // the height of the rectangle

    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a); // setting the colors
    SDL_RenderFillRect(renderer,&board_rect);
}

void Board::draw_onboard(SDL_Renderer *renderer, int32_t row, int32_t col, uint8_t idx, int32_t delta_x, int32_t delta_y){
    // mapping the rect coordenates on the screen
    Color basics = BASE_COLORS[idx];
    Color lights = LIGHT_COLORS[idx];
    Color darks = DARK_COLORS[idx];

    int32_t x = col * GRID_SIZE + delta_x; // mapping x and y on the grid. To find the X values you have to iter the columns
    int32_t y = row * GRID_SIZE + delta_y; // to find the Y values you have to iter at the rows

    fill_board_rect(renderer,x,y,GRID_SIZE,GRID_SIZE,darks); // drawing the first cell
    
    // overdrawing with different colors to make a small render effect of various squares
    int32_t cell_edge = GRID_SIZE/9;
    fill_board_rect(renderer, x + cell_edge, y, GRID_SIZE - cell_edge, GRID_SIZE - cell_edge,lights);
    fill_board_rect(renderer, x + cell_edge , y + cell_edge, GRID_SIZE - cell_edge * 2 , GRID_SIZE - cell_edge * 2, basics);

}

void Board::draw_tetrino(SDL_Renderer *renderer, Tetrino_state *t_state, int32_t delta_x, int32_t delta_y){
    
    const Tetrino *tetrino = TETRINOS + t_state->index; // selecting the type of the tetrino (I,O,L,...)
    assert(tetrino);

    for (int32_t row = 0; row < tetrino->side;++row){
        for (int32_t col = 0; col < tetrino->side;++col){
            uint8_t value = t_state->get_position(tetrino, row, col, t_state->rotation);
            if (value)
            {
                draw_onboard(renderer, row + t_state->offset_row,
                             col + t_state->offset_col,
                             value, delta_x, delta_y);
            }
        }
    }
}

void Board::draw_board(SDL_Renderer *renderer, uint8_t *brd,int32_t offset_x, int32_t offset_y){
    
    for (int32_t row = 0; row < height;++row){
        for (int32_t col = 0; col < width;++col){
            uint8_t value = get_boardmatrix(brd,row,col);
            if (value)
            {
                draw_onboard(renderer, row, col,value, offset_x, offset_y);
            }
        }
    }
}

void Board::render_game(Tetris *tetris_game, SDL_Renderer *renderer){
    draw_board(renderer,tetris_game->board,0,0);
    draw_tetrino(renderer,&tetris_game->piece,0,0);
}

bool Board::check_board_limits(uint8_t* brd,Tetrino_state *tetrino_state){
    const Tetrino *tetrino = TETRINOS + tetrino_state->index;
    assert(tetrino);
    
    for (int32_t row = 0; row < tetrino->side; row++){
        for(int32_t col = 0; col < tetrino->side; col++){
            //get the value of the position of the tetrino
            uint8_t t_position_value = tetrino_state->get_position(tetrino,row,col,tetrino_state->rotation);

            if (t_position_value>0){ // if the position is > 0, than there is part of the piece on this index!!! -> Tetrino detected
                
                int32_t t_position_row = row + tetrino_state->offset_row; // get the exact row position of the respective tetrino
                int32_t t_position_col = col + tetrino_state->offset_col; // get the exact column position of the respective tetrino 

                if(t_position_row < 0){
                    //then it is out of the left bounds of the board
                    return false;
                }

                if(t_position_row >= height){
                    // then it is out of the right bounds of the board
                    return false;
                }

                if(t_position_col < 0){
                    // then it is out of the upper bounds of the board
                    return false;
                }
                
                if(t_position_col >= width){
                    // then it is out of the bottom bounds of the board
                    return false;
                }

                if(get_boardmatrix(brd,t_position_row,t_position_col)){
                    // then there is already something at this exact index on the board! -> COLISION WITH OTHER TETRINOS!
                    return false;
                }

            }
        }
    }

    return true;
}

void Board::update_tetrino_state(Tetris *game, Keyboard *input){
    
    Tetrino_state piece = game->piece;

    if (input->dleft>0){
        -- piece.offset_col;
    }
    if (input->dright>0){
        ++ piece.offset_col;
    }

    if (input->down>0){
        piece.offset_row = piece.offset_row + 1;
    }

    if(input->dup > 0){
        piece.rotation = (piece.rotation + 90) % 360; 
    }

    if (check_board_limits(game->board,&piece)){
        
        game->piece = piece;
    }

    while(game->time > game->next_tetrino_drop_time){ // we do a while because if we skip frames, we drop it several times
        soft_drop(game);
    }


}

void Board::merge_tetrino_into_board(Tetris *game){
    const Tetrino *tetrino = TETRINOS + game->piece.index;
    for(int32_t row = 0; row < tetrino->side; row++){
        for(int32_t col = 0; col < tetrino->side; col++){
            // we get all the non 0 values (values which there is a part of the tetrino) and merge they to the board
            uint8_t position = game->piece.get_position(tetrino,row,col,game->piece.rotation);
            if(position){
                // we translate to the global board coordinates
                int32_t new_brd_row = game->piece.offset_row + row;
                int32_t new_brd_col = game->piece.offset_col + col;
                // and we merge the piece
                set_boardmatrix(game->board,new_brd_row,new_brd_col,position);
            }
        }
    }

}

void Board::spawn_tetrino(Tetris *game){
    game->piece = {};
    game->piece.offset_col = width/2;
}

void Board::soft_drop(Tetris *game){
    ++game->piece.offset_row;
    if(!check_board_limits(game->board,&game->piece)){
        --game->piece.offset_row; // we do it because tha current row is 1 value after the board limits
        // then, now, we merge the piece into the board
        merge_tetrino_into_board(game);
        // and right after that we spawn a new tetrino
        spawn_tetrino(game);
    }

    game->next_tetrino_drop_time = game->time + game->get_next_drop_time();
}

/*LEOZAO AQUI PARCEIRO, TO MANDANDO ESSA MSG PRA DIZER Q VAI DAR TD CERTO E Q CE VAI DESENROLAR ESSE TETRIS, PD FICAR TRANQUILO, TMJ SEMPRE. 
BEIJO NA BUNDA GATAO

ps: tive que dar uma mudada no nome das classes soh pela zueira

NICE!!! 
*/

#endif //TETRIS_TETRIS_H
