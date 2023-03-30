//
// Created by Andre Werneck on 13/12/2022.
//

#ifndef TETRIS_TETRIS_H
#define TETRIS_TETRIS_H

#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iostream>
#include "colors.hpp"
#include "tetrino.hpp" 
#include "frames_per_level.hpp"
#include <assert.h>
#include <random>

#define WIDTH 10
#define HEIGHT 22 // 2 lines invisible to pop up new peaces without show to the user
#define GRID_SIZE 40

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


// =============================================== CLASS KEYBOARD -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================
class Keyboard // take the inputs of the keyboard
{
    public:
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;
    uint8_t h; // enables hard drop 
    
    int8_t dleft;
    int8_t dright;
    int8_t dup;
    int8_t ddown;
    int8_t dh;
};

// =============================================== CLASS TETRIS -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================

enum Tetris_Game_Phase{ // Phases of the Tetris game
    TETRIS_GAME_PLAY, // normal play phase
    TETRIS_GAME_HIGHLIGHT_LINE, // phase in wich we clear a line and highlight it. The game should stop for a really little amount of time
    TETRIS_GAME_OVER // Game over phase 
};

class Tetris
{
    public:
    uint8_t board[WIDTH * HEIGHT];
    uint8_t lines[HEIGHT]; // stores the filled lines
    
    Tetrino_state piece;

    int32_t start_level;
    int32_t level;
    Tetris_Game_Phase phase;
    int32_t filled_line_count; // count the filled lines at each time
    int32_t line_count; // accumulate the filled lines
    int32_t points;

    float time;// absolute time of the game
    float next_tetrino_drop_time; // time to drop the next tetrino
    float highlight_end_time;

    float get_next_drop_time();
    int32_t compute_points();
    int32_t check_lines_to_next_level();
    int32_t max(int32_t x, int32_t y);
    int32_t min(int32_t x, int32_t y);   
};


#endif //TETRIS_TETRIS_H
