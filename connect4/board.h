#pragma once

// DO NOT MODIFY THIS FILE

#include <stdio.h>
#include <stdbool.h>

// the type we will use to represent a position on the board
typedef unsigned char player_t;

// these need to be sequential and in order (and don't modify them)
#define PLAYER_BLUE    2
#define PLAYER_YELLOW  1
#define PLAYER_EMPTY   0

typedef struct board_t board_t;


/// Create a new empty board of the given game configuration.
///    width  = width of the board
///    height = height of the board
///    run    = number of pieces in a row to consider a win
///
/// Returns a new board pointer in *b
/// 
/// If initial is not NULL, then it must point to a (one-dimensional)
/// array of width*height player_t elements containing the initial
/// board configuration, ROW BY ROW, starting with the bottom ROW
/// (i.e. lowest row number, bottom of board),
/// LEFT-TO-RIGHT (i.e. incrementing column numbers).
/// 
/// The board is numbered as follows:
///
/// ROW
///  ...
///  3
///  2
///  1
///  0
///    0 1 2 3 4 COLUMN
/// 
/// In other words, the first row a piece is placed in is row 0
/// And the lower left corner of the board is row 0, column 0
///
/// When initial == NULL, the board should be initialized to be empty.
bool board_create (board_t ** b, unsigned int height,
        unsigned int width, unsigned int run,
        const player_t * initial);


/// Destroy the given board
bool board_destroy (board_t * b);

/// Return the height of the board
unsigned int board_get_height (const board_t * b);

/// Return the width of the board
unsigned int board_get_width (const board_t * b);

/// Return how many pieces need to be in a straight line before the game 
/// is considered to be won. (straight line: horizontal, diagonal or vertical)
unsigned int board_get_run (const board_t * b);

/// Return true if the given move is valid, false otherwise
/// If true, the board was changed and the move was made.
/// If false, the board remains the same.
bool board_play (board_t * b, unsigned int column, player_t player);

/// Return true if the specified player can make a move on the
/// board. THERE IS NO NEED TO KEEP TRACK OF THE TURN OR TO VALIDATE THIS.
/// The board is not changed.
bool board_can_play (const board_t * b, player_t p);

/// Return true if the specified move is valid
/// The board is not changed.
bool board_can_play_move (const board_t * b, player_t p, unsigned int
      column);

/// Undo a move (i.e. make the specified board slot empty)
/// Return true if OK, false on error
/// If the function returns true, the board has been modified.
/// If the function returns false, the board has not been modified.
/// There is no need to keep track of the which player last played.
bool board_unplay (board_t * b, unsigned int column);

/// Duplicate a board
//      return true on success; New board needs to be freed 
///     with board_destroy.
///
/// Return true on success, false on error
bool board_duplicate (board_t ** newboard, const board_t * old);


/// Return -1 if tie, 1 if winner, 0 if no winner
/// If there is a winner, set player to the winning color.
///
/// no winner means that the game can still continue (i.e. open positions)
/// winner means somebody has N tokens in a straight line.
/// tie means there are no more valid moves and there is no winner.
///
/// You can assume that only one of the above conditions is true.
///
int board_has_winner (const board_t * b, player_t * player);

/// Return game piece at specified position
/// Returns false if the position is not valid on the board,
/// returns true otherwise and copies the piece at that location on the board
/// to *piece
bool board_get (const board_t * b, unsigned int row, unsigned int column,
      player_t * piece);


/// Clear the board
/// Return true if success, false otherwise
bool board_clear (board_t * b);

/// Write the board to the given file (for visual aid for the player)
/// Return true if OK, false on error
bool board_print (const board_t * b, FILE * f);
