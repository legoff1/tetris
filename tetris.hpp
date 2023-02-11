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
#include <assert.h>

#define WIDTH 10
#define HEIGHT 24 // 2 lines invisible to pop up new peaces without show to the user
#define VISIBLE_HEIGHT 22
#define GRID_SIZE 30

// =============================================== CLASS TETRINO STATE -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================


class Tetrino_state
{
public:
    uint8_t index; // index to inform which tetrino is it (S,T,O,I, etc)
    int32_t offset_row; // row and column to indetify where the piece currently is in the gme field (board)
    int32_t offset_col;
    int32_t rotation;
    explicit Tetrino_state(uint8_t idx, int32_t os_row,int32_t os_col, int32_t rot): index(idx), offset_row(os_row), offset_col(os_col),rotation(rot){}
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


// =============================================== CLASS BOARD -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================

class Board{
private:
    const int32_t width;
    const int32_t height;
    uint8_t* board;
public:
    explicit Board(): width(10*40), height(20*40),board(nullptr){} //setting the constructor

    const int32_t get_width(); // get the width of the board
    const int32_t get_height(); // get the height of the board
    uint8_t* set_board(); // creates a board with the right dimensions of the game - projection a 2d array in a same dimension 1d array
    uint8_t get_boardmatrix(int32_t row, int32_t col) const; // get the value of an index of the board from a 2d matrix
    void set_boardmatrix(int32_t row, int32_t col, uint8_t value); // set the value of an index of the board in a 2d matrix
    int create_window(); // create and displays a window with SDL2
    void fill_board_rect(SDL_Renderer *renderer, int32_t coord_x, int32_t coord_y, int32_t w, int32_t h, Color color); // just filling the board with the rect method of SDL to get the tetrinos
    void draw_onboard(SDL_Renderer *renderer, int32_t row, int32_t col, uint8_t value, int32_t delta_x, int32_t delta_y); // now we are drawing the rect (cells/pieces) filled above
    void draw_tetrino(SDL_Renderer *renderer, Tetrino_state *t_state, int32_t delta_x, int32_t delta_y); // drawing and rendering the tetrinos using the draw_onboard method
    void render_game(Tetrino_state *tetris_game, SDL_Renderer *renderer); // uses the functions above to renderr the game itself with the pieces and other features
    bool check_board_limits(Tetrino_state *tetrino_state); // set the limits of the board and check if the piece is at the bounderies or in colision with another piece.

};

const int32_t Board::get_width(){
    return width;
}

const int32_t Board::get_height(){
    return height; 
}

uint8_t* Board::set_board(){
    board[width * height];
    return board;
}

uint8_t Board::get_boardmatrix(int32_t row, int32_t col) const{
    int32_t index = row * width + col;
    return board[index]; // tells us if there is or not any cell placed at the index on the board! 
}

void Board::set_boardmatrix(int32_t row, int32_t col, uint8_t value){
    int32_t index = row * width + col;
    board[index] = value;
}

int Board::create_window(){

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        "Tetris",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC
    );


    if(NULL == window){
        std::cout << "Error Detected" << std::endl;
        return 1;
    }

    Tetrino_state tet(2,0,0,90);
    Tetrino_state tet1(1,5,5,0);

    while(true){
        SDL_Event windowEvent;
        if (SDL_PollEvent(&windowEvent)){
            if (SDL_QUIT==windowEvent.type){
                std::cout << "entrou" << std::endl;
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_RenderClear(renderer);
        render_game(&tet,renderer);
        render_game(&tet1,renderer);
        SDL_RenderPresent(renderer);
        tet.offset_row += 1;
        // tet.rotation = (tet.rotation + 90) % 360;
        SDL_Delay(500);
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
    int32_t cell_edge = GRID_SIZE/8;
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

void Board::render_game(Tetrino_state *tetris_game, SDL_Renderer *renderer){
    draw_tetrino(renderer,tetris_game,0,0);
}

bool Board::check_board_limits(Tetrino_state *tetrino_state){
    const Tetrino *tetrino = TETRINOS + tetrino_state->index;
    assert(tetrino);
    
    for (int32_t row = 0; row < height; row++){
        for(int32_t col = 0; col < width; col++){
            //get the value of the position of the tetrino
            uint8_t t_position_value = tetrino_state->get_position(tetrino,row,col,tetrino_state->rotation);

            if (t_position_value>0){ // if the position is > 0, than there is part of the piece on this index!!! -> Tetrino detected
                
                int32_t t_position_row = row + tetrino_state->offset_row; // get the exact row position of the respective tetrino
                int32_t t_position_col = col + tetrino_state->offset_col; // get the exact column position of the respective tetrino 

                if(t_position_row < 0){
                    //then it is out of the left bounds of the board
                    return false;
                }

                if(t_position_row > width){
                    // then it is out of the right bounds of the board
                    return false;
                }

                if(t_position_col < 0){
                    // then it is out of the upper bounds of the board
                    return false;
                }
                
                if(t_position_col > height){
                    // then it is out of the bottom bounds of the board
                    return false;
                }

                if(get_boardmatrix(t_position_row,t_position_col)){
                    // then there is already something at this exact index on the board! -> COLISION WITH OTHER TETRINOS!
                    return false;
                }

            }
        }
    }

    return true;
}

// =============================================== CLASS KEYBOARD -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================

class Keyboard{
    private:
        int8_t go_left;
        int8_t go_right;
        int8_t go_down;
        int8_t rotate;
    public:
        void update_position(Tetrino_state *tetrino_state); // update positions according the the inputs on keyboard 

};

// =============================================== CLASS TETRIS -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================

class Tetris{
 public:
    Keyboard keyboard;
    Board board;
    Tetrino_state tetrino_state;
};



/*LEOZAO AQUI PARCEIRO, TO MANDANDO ESSA MSG PRA DIZER Q VAI DAR TD CERTO E Q CE VAI DESENROLAR ESSE TETRIS, PD FICAR TRANQUILO, TMJ SEMPRE. 
BEIJO NA BUNDA GATAO

ps: tive que dar uma mudada no nome das classes soh pela zueira

NICE!!! 
*/

#endif //TETRIS_TETRIS_H
