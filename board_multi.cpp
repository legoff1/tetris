//
// Created by Andre Werneck on 13/12/2022.
//

#include "board.hpp"

#include <../SFML/Network.hpp>
#include "../network/Client.hpp"

Client client;

// =============================================== CLASS BOARD -> DEFINITION AND FUNCTION IMPLEMENTATIONS ==============================================

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
    
    if(TTF_Init() < 0){       
        return 2;
    }
    
    SDL_Window *window = SDL_CreateWindow(
        "Tetris",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        (width)*GRID_SIZE + 200,
        (height)*GRID_SIZE + 40,
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC
    );


    //test
    SDL_Window *window2 = SDL_CreateWindow(
        "Tetris autre joeur",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        (width)*GRID_SIZE,
        (height)*GRID_SIZE + 30,
        SDL_WINDOW_ALLOW_HIGHDPI
    );
    SDL_Renderer *renderer2 = SDL_CreateRenderer(
        window2,
        -1,
        SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC
    );
    //fin du test

    const char *used_font = "novem___.ttf";
    TTF_Font *f = TTF_OpenFont(used_font,25);

    Tetris game = {};
    //test
    Tetris game2 = {};
    //fin du test
    Keyboard input = {};
    
    game.hold_piece = false; //hold starts as false until H is pressed
    game.spawn_tetrino();

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

        RESPONSE_STATUS status;

        if(key_states[SDL_SCANCODE_C]){
            status = client.listenConection();
        }

        if(key_states[SDL_SCANCODE_J]){
            status = client.searchConection();
        }

        Keyboard old_input = input;

        input.left = key_states[SDL_SCANCODE_LEFT];
        input.right = key_states[SDL_SCANCODE_RIGHT];
        input.up = key_states[SDL_SCANCODE_UP];
        input.down = key_states[SDL_SCANCODE_DOWN];
        input.space = key_states[SDL_SCANCODE_SPACE];
        input.hold = key_states[SDL_SCANCODE_H];

        input.dleft = input.left - old_input.left;
        input.dright = input.right - old_input.right;
        input.dup = input.up - old_input.up;
        input.ddown = input.down - old_input.down;
        input.dspace = input.space - old_input.space;
        input.dhold = input.hold - old_input.hold;
        
        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_RenderClear(renderer);

        //test
        SDL_SetRenderDrawColor(renderer2,0,0,0,0);
        SDL_RenderClear(renderer2);
        // fin du test
        
        update_tetris_game(&game,&input);
        render_game(&game,renderer,f);


        //test
        if (status == RESPONSE_STATUS::ESTABLISHED_CONNECTION){
        client.sendData(game.board);
        client.sendDataInt(game.points);
        client.sendDataInt(game.line_count);
        client.sendDataInt(game.level);



        client.recvData(game2.board);
        client.recvDataInt(game2.points);
        client.recvDataInt(game2.line_count);
        client.recvDataInt(game2.level);
        }

        render_game(&game2,renderer2,f);
        SDL_RenderPresent(renderer2);
        //fin du test
        
        SDL_RenderPresent(renderer);
        
    }
    TTF_CloseFont(f);
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

void Board::draw_onboard(SDL_Renderer *renderer, int32_t row, int32_t col, uint8_t idx, int32_t delta_x, int32_t delta_y, bool gost_tetrino=false){
    // mapping the rect coordenates on the screen
    Color basics = BASE_COLORS[idx];
    Color lights = LIGHT_COLORS[idx];
    Color darks = DARK_COLORS[idx];

    int32_t x = col * GRID_SIZE + delta_x; // mapping x and y on the grid. To find the X values you have to iter the columns
    int32_t y = row * GRID_SIZE + delta_y; // to find the Y values you have to iter at the rows

    if(gost_tetrino == true){

        draw_gost_tetrino(renderer,x,y,GRID_SIZE,GRID_SIZE,basics);
        return;

    }

    
    fill_board_rect(renderer,x,y,GRID_SIZE,GRID_SIZE,darks); // drawing the first cell
    
    // overdrawing with different colors to make a small render effect of various squares
    int32_t cell_edge = GRID_SIZE/9;
    fill_board_rect(renderer, x + cell_edge, y, GRID_SIZE - cell_edge, GRID_SIZE - cell_edge,lights);
    fill_board_rect(renderer, x + cell_edge , y + cell_edge, GRID_SIZE - cell_edge * 2 , GRID_SIZE - cell_edge * 2, basics);

}

void Board::draw_tetrino(SDL_Renderer *renderer, Tetrino_state *t_state, int32_t delta_x, int32_t delta_y, bool gost_tetrino = false){
    
    const Tetrino *tetrino = TETRINOS + t_state->index; // selecting the type of the tetrino (I,O,L,...)
    assert(tetrino);

    for (int32_t row = 0; row < tetrino->side;++row){
        for (int32_t col = 0; col < tetrino->side;++col){
            uint8_t value = t_state->get_position(tetrino, row, col, t_state->rotation);
            if (value)
            {
                draw_onboard(renderer, row + t_state->offset_row,
                             col + t_state->offset_col,
                             value, delta_x, delta_y, gost_tetrino);
            }
        }
    }
}

void Board::draw_gost_tetrino(SDL_Renderer *renderer, int32_t coord_x, int32_t coord_y, int32_t w, int32_t h, Color color)
{
    SDL_Rect board_rect={};
    
    board_rect.x = coord_x; // the x location of the rectangle's upper left corner
    board_rect.y = coord_y; // the y location of the rectangle's upper left corner
    board_rect.w = w; // the width of the rectangle
    board_rect.h = h; // the height of the rectangle

    SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a); // setting the colors
    SDL_RenderDrawRect(renderer,&board_rect);
}

void Board::draw_board(SDL_Renderer *renderer, uint8_t *brd,int32_t offset_x, int32_t offset_y){
    
    for (int32_t row = 0; row < height;++row){
        for (int32_t col = 0; col < width;++col){
            uint8_t value = get_boardmatrix(brd,row,col);
            draw_onboard(renderer,row,col,value,offset_x,offset_y);
            if (value)
            {
                draw_onboard(renderer, row, col,value, offset_x, offset_y);
            }
        }
    }
}

void Board::draw_text(SDL_Renderer *renderer,TTF_Font *f, const char *text, int32_t x, int32_t y, Text_Align alignment, Color color){
    // some SDL formalism to plot the text (as I've seen on a SDL tutorial)
    SDL_Color c = SDL_Color {color.r,color.g,color.b,color.a};
    SDL_Surface *surface = TTF_RenderText_Solid(f,text,c);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer,surface);
    SDL_Rect rect;
    rect.w = surface->w;
    rect.h = surface->h;
    

    switch (alignment){
        case TEXT_LEFT: rect.x = x;
        rect.y = y;
        break;

        case TEXT_RIGHT: rect.x = x - surface->w;
        rect.y = y;
        break;

        case TEXT_CENTER:rect.x = x - surface->w/2;
        rect.y = y;
        break;
    }

    SDL_RenderCopy(renderer,texture,0,&rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


void Board::render_game(Tetris *tetris_game, SDL_Renderer *renderer,TTF_Font *f){
    
    int32_t margin_on_top = 40;
    char buffer[4096];
    
    draw_board(renderer,tetris_game->board,0,margin_on_top);

    // just draw the tetrino if the game phase is play!!!
    if((tetris_game->phase == TETRIS_GAME_PLAY) || (tetris_game->phase==TETRIS_GAME_HIGHLIGHT_LINE)){

        sprintf(buffer,"NEXT PIECE");
        draw_text(renderer,f,buffer,(width+17)*GRID_SIZE/2,(height/2)*GRID_SIZE/2,TEXT_CENTER,Color(0xFF,0xFF,0xFF,0xFF));

        sprintf(buffer,"PIECE HOLDED");
        draw_text(renderer,f,buffer,(width+17)*GRID_SIZE/2,(height+2)*GRID_SIZE/2,TEXT_CENTER,Color(0xFF,0xFF,0xFF,0xFF));
        // if there is already a holded piece we plot it
        if(tetris_game->hold_piece==true){
            draw_tetrino(renderer,&tetris_game->holded_piece,width*GRID_SIZE/2 + 212, (height+5)*GRID_SIZE/2);
        }

        draw_tetrino(renderer,&tetris_game->piece,0,margin_on_top);

        if(tetris_game->piece.index==0){
            //draw the next piece
            draw_tetrino(renderer,&tetris_game->next_piece,(width)*GRID_SIZE/2+212,(height/2)*GRID_SIZE/2+50);    
        }
        else if(tetris_game->piece.index==1){
            //draw the next piece
            draw_tetrino(renderer,&tetris_game->next_piece,width*GRID_SIZE/2 + 212,(height/2)*GRID_SIZE/2+50);    
        }
        else{
            //draw the next piece
            draw_tetrino(renderer,&tetris_game->next_piece,width*GRID_SIZE/2 + 212,(height/2)*GRID_SIZE/2+50);
        }

        Tetrino_state piece = tetris_game->piece; // copying the piece to draw it as a gost piece

        // we will check the possible places for the gost (also the actual piece) at the bottom of the board
        while(check_board_limits(tetris_game->board,&piece)){
            // while we have space, we incrememt the position
            piece.offset_row++;
        }
        --piece.offset_row;

        //draw the gost piece
        draw_tetrino(renderer,&piece,0,margin_on_top,true);

    }
    
    if(tetris_game->phase == TETRIS_GAME_HIGHLIGHT_LINE){
        for(int32_t row=0; row < height; ++row){
            if(tetris_game->lines[row]){ // if the line is between the filled lines we highlight it
                int32_t x =0;
                int32_t y = row * GRID_SIZE + margin_on_top ;
                fill_board_rect(renderer,x,y,width * GRID_SIZE ,GRID_SIZE,Color(0xFF,0xFF,0xFF,0xFF)); // highlighting with white
            }
        }
    }

    else if(tetris_game->phase==TETRIS_GAME_OVER){
        draw_text(renderer,f,"GAME OVER!!!",width*GRID_SIZE/2,height*GRID_SIZE/2,TEXT_CENTER,Color(0xFF,0xFF,0xFF,0xFF));
    }

    else if(tetris_game->phase == TETRIS_GAME_START){
        
        draw_text(renderer,f,"PRESS SPACEBAR",width*GRID_SIZE/2, 
        (height + (height-VISIBLE_HIGHT))*GRID_SIZE/2,TEXT_CENTER,Color(0xFF,0xFF,0xFF,0xFF));

        draw_text(renderer,f,"TO START THE GAME!!!",width*GRID_SIZE/2, 
        (height + (2+height-VISIBLE_HIGHT))*GRID_SIZE/2,TEXT_CENTER,Color(0xFF,0xFF,0xFF,0xFF));

        sprintf(buffer,"STARTING LEVEL = %d ",tetris_game->start_level);
        draw_text(renderer,f,buffer,width*GRID_SIZE/2, 
        (height + (4+height-VISIBLE_HIGHT))*GRID_SIZE/2,TEXT_CENTER,Color(0xFF,0xFF,0xFF,0xFF));

        draw_text(renderer,f,"PRESS C TO CREATE MULTI",width*GRID_SIZE/2, 
        (height + (6+height-VISIBLE_HIGHT))*GRID_SIZE/2,TEXT_CENTER,Color(0xFF,0xFF,0xFF,0xFF));

        draw_text(renderer,f,"PRESS J TO JOIN MULTI",width*GRID_SIZE/2, 
        (height + (8+height-VISIBLE_HIGHT))*GRID_SIZE/2,TEXT_CENTER,Color(0xFF,0xFF,0xFF,0xFF));

    }
    // colors the invisible lines of black
    fill_board_rect(renderer,0,margin_on_top,width*GRID_SIZE,(height-VISIBLE_HIGHT)*GRID_SIZE,Color(0x00,0x00,0x00,0x00)); // set just the visible rows to the user to see it
    
    sprintf(buffer,"POINTS = %d ",tetris_game->points);
    draw_text(renderer,f,buffer,15,10,TEXT_LEFT,Color(0xFF,0xFF,0xFF,0xFF));

    sprintf(buffer,"FILLED LINES = %d ",tetris_game->line_count);
    draw_text(renderer,f,buffer,15,40,TEXT_LEFT,Color(0xFF,0xFF,0xFF,0xFF));
    
    sprintf(buffer,"LEVEL = %d",tetris_game->level);
    draw_text(renderer,f,buffer,15,70,TEXT_LEFT,Color(0xFF,0xFF,0xFF,0xFF));
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

                if(t_position_col < 0){
                    //then it is out of the left bounds of the board
                    return false;
                }

                if(t_position_col >= width){
                    // then it is out of the right bounds of the board
                    return false;
                }

                if(t_position_row < 0){
                    // then it is out of the upper bounds of the board
                    return false;
                }
                
                if(t_position_row >= height){
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

void Board::update_tetrisgame_state(Tetris *game, Keyboard *input){
    
    Tetrino_state piece = game->piece;

    if (input->dleft>0){
        -- piece.offset_col;
    }
    if (input->dright>0){
        ++ piece.offset_col;
    }

    if (input->ddown>0){
        piece.offset_row = piece.offset_row + 1;
    }

    if(input->dup > 0){
        piece.rotation = (piece.rotation + 90) % 360; 
    }
 
    if((input->dhold > 0) && (game->hold_valid==true)){
        game->phase = TETRIS_HOLD_PIECE;
    }

    if (check_board_limits(game->board,&piece)){
        
        game->piece = piece;
    }

    if(input->dspace > 0){
        
        while(soft_drop(game)); // we make our hard drop while we can with the SPACE BAR on keyboard
    }

    while(game->time >= game->next_tetrino_drop_time){ // we do a while because if we skip frames, we drop it several times
        soft_drop(game);
    }

    game->filled_line_count = check_and_count_lines(game->board,game->lines); // if we have filled one line, we switch the phase
    if(game->filled_line_count > 0){
        // time to switch the phase to highlight and delete the line
        game->phase = TETRIS_GAME_HIGHLIGHT_LINE;
        game->highlight_end_time = game->time + 0.6f; // time to block the game play
    }

    int32_t first_row = 0;
    if(!see_row_empty(game->board,first_row)){
        
        game->phase = TETRIS_GAME_OVER;
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

bool Board::soft_drop(Tetris *game){
    
    ++game->piece.offset_row;
    if(!check_board_limits(game->board,&game->piece)){
        --game->piece.offset_row; // we do it because tha current row is 1 value after the board limits
        // then, now, we merge the piece into the board
        merge_tetrino_into_board(game);
        game->hold_valid = true;
        // and right after that we spawn a new tetrino
        game->spawn_tetrino();
        return false;
    }
    game->next_tetrino_drop_time = game->time + game->get_next_drop_time();
    return true;
}

uint8_t Board::see_row_filled(uint8_t *board, int32_t row_number){
    // iterate on the rows
    for(int32_t col=0; col < width; col++){
        if(!get_boardmatrix(board,row_number,col)){ // if the cell is empty:
         
            return 0;
        }
    }
    return 1; // if we go through all the columns and none of them is empty, so we return 1 meaning that all the line is filled
}

uint8_t Board::see_row_empty(uint8_t *board, int32_t row_number){
    for(int32_t col=0; col< width; col++){
        if(get_boardmatrix(board,row_number,col)){ // check if the cell is not empty
            return 0;
        }
    }

    return 1; // if there is at least one empty column, returns 1 
}

int32_t Board::check_and_count_lines(uint8_t *board, uint8_t *lines_that_got_out){
    int32_t count = 0;
    // iterate on the lines
    for(int32_t row = 0; row < height; row++){
        uint8_t filled = see_row_filled(board,row); // check each column of the row and set 1 if its filled
        lines_that_got_out[row] = filled;
        count+=filled; // filled is 1 (for lines out) or 0 (on the other hand) 
    }
    return count;
}

void Board::delete_lines(uint8_t *board,uint8_t *lines){
    // we create 2 cursors iterating on the rows: source(from old board) and destination(from new board)
    int32_t source = height - 1; // bottom of the old board
    // destination also initiates at the bottom of the new board
    for (int32_t destination = height-1; destination >=0; destination--){
        //while source row has not reached the top of the board and the lines at the same level or above are filled
        while(source>=0 && lines[source]){
            --source;
        }
        if(source<0){ // has reached the top of the board from source and then we need to set the rest of destination with empty lines
            memset(board + destination * width,0,width);
        }
        else{// we copy the cells that are not fully filled to the destination
            memcpy(board + destination*width, board + source*width,width);
            --source; // keep going until we reach the top of the board
        }
    }
}

void Board::update_board_lines(Tetris *game){
    if(game->time >= game->highlight_end_time){
        // if the lines are already highlighted, we delete them
        delete_lines(game->board,game->lines);
        game->line_count += game->filled_line_count; // accumulate the lines deleted to compute the points
        game->points += game->compute_points(); // computing the points
        int32_t lines_for_next_level = game->check_lines_to_next_level(); // check if we have enough lines to go to the next level
        if(game->line_count >= lines_for_next_level){
            ++game->level;
        }
        // and after all we return to the game play
        game->phase = TETRIS_GAME_PLAY;
    }
}

void Board::update_tetris_game(Tetris *game, Keyboard *input){
    switch(game->phase){
        case TETRIS_GAME_START : return game->update_game_start_state(input);
        break;
        case TETRIS_GAME_PLAY : return update_tetrisgame_state(game,input); // goes updating the tetrinos on the board and play the normal game
        break;
        case TETRIS_HOLD_PIECE: return game->handle_hold_state(); // handles the hold function
        break;
        case TETRIS_GAME_HIGHLIGHT_LINE : return update_board_lines(game); // moment when a line is filled -> blocks the game for a little while
        break;
        case TETRIS_GAME_OVER : return game->update_game_over_state(input);
        break;
    }
}

