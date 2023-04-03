//
// Created by Andre Werneck on 13/12/2022.
//

#ifndef BOARD_TETRIS_H
#define BOARD_TETRIS_H

#include "tetris.hpp"

// =============================================== CLASS BOARD -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================

enum Text_Align{
    TEXT_LEFT,
    TEXT_CENTER,
    TEXT_RIGHT
};

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
    void draw_onboard(SDL_Renderer *renderer, int32_t row, int32_t col, uint8_t value, int32_t delta_x, int32_t delta_y, bool gost_tetrino); // now we are drawing the rect (cells/pieces) filled above
    void draw_tetrino(SDL_Renderer *renderer, Tetrino_state *t_state, int32_t delta_x, int32_t delta_y, bool gost_tetrino); // drawing and rendering the JUST tetrinos using the draw_onboard method
    void draw_gost_tetrino(SDL_Renderer *renderer, int32_t coord_x, int32_t coord_y, int32_t w, int32_t h, Color color); // just draw the gost tetrino on the board. Uses sdl draw rect instead of fill rect
    void draw_board(SDL_Renderer *renderer, uint8_t *brd, int32_t offset_x, int32_t offset_y); // draw the game board WITHIN the tetrinos present on it
    void draw_text(SDL_Renderer *renderer,TTF_Font *f, const char *text, int32_t x, int32_t y, Text_Align alignment, Color color); // draw the text on the screen
    void render_game(Tetris *tetris_game, SDL_Renderer *renderer, TTF_Font *f); // uses the functions above to renderr the game itself with the pieces and other features
    bool check_board_limits(uint8_t* brd,Tetrino_state *tetrino_state); // set the limits of the board and check if the piece is at the bounderies or in colision with another piece.
    void update_tetrisgame_state(Tetris *game, Keyboard *input); // update positions according the the inputs on keyboard
    bool soft_drop(Tetris *game); // do the soft drop of a piece in the board. If the piece reaches the board limits, we merge it to the board.
    void merge_tetrino_into_board(Tetris *game); // merges the tetrino into board when it reaches board`s limit.
    int32_t check_and_count_lines(uint8_t *board,uint8_t *lines_that_got_out); // will see if the line is filled and if yes, will count the number of filled lines
    uint8_t see_row_filled(uint8_t *board, int32_t row_number); // just check if the row is filled or not -> will be used on the function above
    uint8_t see_row_empty(uint8_t *board, int32_t row_number); // just check if the row is empty or not -> will be used to check game over condition
    void delete_lines(uint8_t *board,uint8_t *lines); // delete the filled lines
    void update_board_lines(Tetris *game); // call the delete lines function to update the board when a line is filled
    void update_tetris_game(Tetris *game, Keyboard *input); // update the game play with the existent phases
};


#endif //BOARD_TETRIS_H
