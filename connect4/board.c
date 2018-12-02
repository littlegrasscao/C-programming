#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
/**
 * In this file, implement all of the functions for board.h
 *
 * Once done, make sure you pass all the tests in board_test.c
 * (make test_board)
 *
 */


struct board_t{
    unsigned int number; // current number of pieces on the board
    unsigned int size;   //size of the board
    unsigned int width;  //    width  = width of the board
    unsigned int height; //    height = height of the board
    unsigned int run;    //    run    = number of pieces in a row to consider a win
    unsigned char * board; //an integer column used to indicates board 
};

/// Create a new empty board of the given game configuration.
bool board_create (board_t ** b, unsigned int height, unsigned int width, unsigned int run, const player_t * initial){
    //malloc struct
    (*b) = (board_t *)malloc(sizeof(board_t));    
    (*b)->height = height;
    (*b)->width = width;
    (*b)->run = run;

    //malloc board
    unsigned int size = height*width;
    (*b)->size = size;
    (*b)->board = (unsigned char *)malloc(sizeof(unsigned char) * (size+1));
    
    //if given initial, set board to initial
    if(initial){
        (*b)->number = size; 
        memcpy((*b)->board,initial,size);
    }
    else{
        //set board to empty
        (*b)->number = 0;
        for(unsigned int i=0;i<size;i++){
            (*b)->board[i] = PLAYER_EMPTY;
        }
    }
    (*b)->board[size] = '\0';

    return true;
}


/// Destroy the given board
bool board_destroy (board_t * b){
    if(b == NULL)
        return false;
    
    if(b->board)
        free(b->board);
    free(b);
    return true;
}

/// Return the height of the board
unsigned int board_get_height (const board_t * b){
    return b->height;
}

/// Return the width of the board
unsigned int board_get_width (const board_t * b){
    return b->width;
}

/// Return how many pieces need to be in a straight line before the game 
/// is considered to be won. (straight line: horizontal, diagonal or vertical)
unsigned int board_get_run (const board_t * b){
    return b->run;
}

/// Return true if the given move is valid, false otherwise
/// If true, the board was changed and the move was made.
/// If false, the board remains the same.
bool board_play (board_t * b, unsigned int column, player_t player){
    //if the column has an empty position
    if(board_can_play_move(b,player,column)){
        unsigned int i = column; 
        //find the first empty position and set to player
        while(i < b->size){
            if (b->board[i] == PLAYER_EMPTY){
                b->board[i] = player;
                b->number++;
                return true;
            }
            i += b->width;
        }        
    }

    return false;
}

/// Return true if the specified player can make a move on the
/// board. THERE IS NO NEED TO KEEP TRACK OF THE TURN OR TO VALIDATE THIS.
/// The board is not changed.
bool board_can_play (const board_t * b, player_t p){
    //compare current number with size
    if (b->number < b->size){
        return true;
    }
    else{
        return false;
    }
}

/// Return true if the specified move is valid
/// The board is not changed.
bool board_can_play_move (const board_t * b, player_t p, unsigned int column){
    //if the column has an empty position
    unsigned int i = column; 
    while(i < b->size){
        if (b->board[i] == PLAYER_EMPTY){
            return true;
        }
        i += b->width;
    }
    
    return false;
}

/// Undo a move (i.e. make the specified board slot empty)
/// Return true if OK, false on error
/// If the function returns true, the board has been modified.
/// If the function returns false, the board has not been modified.
/// There is no need to keep track of the which player last played.
bool board_unplay (board_t * b, unsigned int column){
    //if the column of first row is empty, return false
    if(b->board[column] == PLAYER_EMPTY){
        return false;
    }
    else{
        //find the unempty field in the column with highest row
        unsigned int i = column; 
        while(i < b->size){
            if (b->board[i] == PLAYER_EMPTY){
                break;
            }
            i += b->width;
        }
        //change to empty
        b->board[i-=b->width] = PLAYER_EMPTY;
        b->number--;
        return true;
    }
}

/// Duplicate a board
//      return true on success; New board needs to be freed 
///     with board_destroy.
///
/// Return true on success, false on error
bool board_duplicate (board_t ** newboard, const board_t * old){
    if(old == NULL || old->board == NULL){
        return false;
    }

    //create newboard
    (*newboard) = (board_t *)malloc(sizeof(board_t));    
    (*newboard)->height = old->height;
    (*newboard)->width = old->width;
    (*newboard)->run = old->run;
    
    //malloc board
    (*newboard)->board = (unsigned char *)malloc(sizeof(unsigned char) * (old->size+1));
    //set board to old->board
    (*newboard)->number = old->number; 
    (*newboard)->size = old->size;
    memcpy((*newboard)->board,old->board,old->size+1);
    
    // for(unsigned int i=0; i<old->size; i++){
    //     (*newboard)->board[i] = old->board[i];
    // }
    // (*newboard)->board[] = '\0';
    return true;    
}


/// Return -1 if tie, 1 if winner, 0 if no winner
/// If there is a winner, set player to the winning color.
///
/// no winner means that the game can still continue (i.e. open positions)
/// winner means somebody has N tokens in a straight line.
/// tie means there are no more valid moves and there is no winner.
///
/// You can assume that only one of the above conditions is true.
///
int board_has_winner (const board_t * b, player_t * player){
    // horizontalCheck 
    for (int i=0; i<b->height; i++){
        int blue=0, yellow=0; //initial count to 0
        for (int j = 0; j<b->width; j++){
            //current piece is blue
            if (b->board[i*b->width+j] == PLAYER_BLUE){
                blue++;
                yellow = 0;
            }
            //current piece is yellow
            else if(b->board[i*b->width+j] == PLAYER_YELLOW){
                yellow++;
                blue = 0;
            }
            else{ //empty 
                blue = 0;
                yellow = 0;
            }
            //check blue and yellow count
            if(blue==b->run){
                *player = PLAYER_BLUE;
                return 1;
            }
            else if(yellow==b->run){
                *player = PLAYER_YELLOW;
                return 1;
            }
        }           
    }

    // verticalCheck
    for (int j = 0; j<b->width; j++){
        int blue=0, yellow=0; //initial count to 0
        for (int i=0; i<b->height; i++){
            //current piece is blue
            if (b->board[i*b->width+j] == PLAYER_BLUE){
                blue++;
                yellow = 0;
            }
            //current piece is yellow
            else if(b->board[i*b->width+j] == PLAYER_YELLOW){
                yellow++;
                blue = 0;
            }
            else{ //empty 
                blue = 0;
                yellow = 0;
            }
            //check blue and yellow count
            if(blue==b->run){
                *player = PLAYER_BLUE;
                return 1;
            }
            else if(yellow==b->run){
                *player = PLAYER_YELLOW;
                return 1;
            }
        }           
    }

    //bot-left to top-right(top part)
    for( int rowStart = 0; rowStart < b->height; rowStart++){
        int blue=0, yellow=0;
        int row, col;
        //check each piece along the top part of the diagonal.each time move to the right-top one.
        for( row = rowStart, col = 0; row < b->height && col < b->width; row++, col++ ){
            if(b->board[row*b->width+col] == PLAYER_BLUE){
                blue++;
                yellow = 0;
            }
            else if(b->board[row*b->width+col] == PLAYER_YELLOW){
                yellow++;
                blue = 0;
            }
            else{ //empty 
                blue = 0;
                yellow = 0;
            }
            //check blue and yellow count
            if(blue==b->run){
                *player = PLAYER_BLUE;
                return 1;
            }
            else if(yellow==b->run){
                *player = PLAYER_YELLOW;
                return 1;
            }
        }
    }

    //bot-left to top-right(bot part)
    for(int colStart = 1; colStart < b->width; colStart++){
        int blue=0, yellow=0;
        int row, col;
        //check each piece along the bot part of the diagonal.each time move to the right-top one.
        for( row = 0, col = colStart; row < b->height && col < b->width; row++, col++ ){
            if(b->board[row*b->width+col] == PLAYER_BLUE){
                blue++;
                yellow = 0;
            }
            else if(b->board[row*b->width+col] == PLAYER_YELLOW){
                yellow++;
                blue = 0;
            }
            else{ //empty 
                blue = 0;
                yellow = 0;
            }
            //check blue and yellow count
            if(blue==b->run){
                *player = PLAYER_BLUE;
                return 1;
            }
            else if(yellow==b->run){
                *player = PLAYER_YELLOW;
                return 1;
            }
        }
    }

    //top-left to bot-right(bot part)
    for( int rowStart = b->height-1; rowStart > 0; rowStart--){
        int blue=0, yellow=0;
        int row, col;
        //check each piece along the bot part of the diagonal. each time move to the right-bot one.
        for( row = rowStart, col = 0; row >= 0 && col < b->width; row--, col++ ){
            if(b->board[row*b->width+col] == PLAYER_BLUE){
                blue++;
                yellow = 0;
            }
            else if(b->board[row*b->width+col] == PLAYER_YELLOW){
                yellow++;
                blue = 0;
            }
            else{ //empty 
                blue = 0;
                yellow = 0;
            }
            //check blue and yellow count
            if(blue==b->run){
                *player = PLAYER_BLUE;
                return 1;
            }
            else if(yellow==b->run){
                *player = PLAYER_YELLOW;
                return 1;
            }
        }
    }

    //top-left to bot-right(top part)
    for(int colStart = 1; colStart < b->width; colStart++){
        int blue=0, yellow=0;
        int row, col;
        //check each piece along the top part of the diagonal. each time move to the right-bot one.
        for( row = b->height-1, col = colStart; row >= 0 && col < b->width; row--, col++ ){
            if(b->board[row*b->width+col] == PLAYER_BLUE){
                blue++;
                yellow = 0;
            }
            else if(b->board[row*b->width+col] == PLAYER_YELLOW){
                yellow++;
                blue = 0;
            }
            else{ //empty 
                blue = 0;
                yellow = 0;
            }
            //check blue and yellow count
            if(blue==b->run){
                *player = PLAYER_BLUE;
                return 1;
            }
            else if(yellow==b->run){
                *player = PLAYER_YELLOW;
                return 1;
            }
        }
    }

    //tie if board is full and there is no winner
    if(b->number == b->size){
        return -1;
    }
    //no winner, if current number < size
    else{
        return 0;
    }  
}

/// Return game piece at specified position
/// Returns false if the position is not valid on the board,
/// returns true otherwise and copies the piece at that location on the board
/// to *piece
bool board_get (const board_t * b, unsigned int row, unsigned int column, player_t * piece){
    unsigned int position = row*b->width + column;
    if(b==NULL || b->board==NULL || row >= b->height || column >= b->width){
        return false;
    }
    else{
        *piece = b->board[position];
        return true;
    }
}


/// Clear the board
/// Return true if success, false otherwise
bool board_clear (board_t * b){
    if(b == NULL || b->board == NULL){
        return false;
    }
    //set board to empty
    for(unsigned int i=0; i<b->size; i++){
        b->board[i] = PLAYER_EMPTY;
    }
    b->number=0;
    return true;
}

/// Write the board to the given file (for visual aid for the player)
/// Return true if OK, false on error
bool board_print (const board_t * b, FILE * f){
    if(b == NULL || b->board == NULL){
        return false;
    }

    //print board from top to bot
    int row, col;
    for( row=b->height-1; row >= 0; row--){
        for (col=0; col<b->width; col++){
            fprintf(f, "%d", (int)b->board[row*b->width + col]);
            fprintf(f," ");
        }
        fprintf(f,"\n");
    }
    return true;
}

