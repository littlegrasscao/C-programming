#include "computer_agent.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
/*
 * In this file, implement your AI agent.
 *
 * Use the minimax algorithm to determine the next move.
 *
 * Look at player_agent.h and player_agent.c for an example of how to
 * implement each function.
 */

//store name
typedef struct
{
   char * name;
} computer_agent_data_t;

//get the name of agent
static const char * computer_agent_describe (agent_t * gthis)
{
   computer_agent_data_t * data = (computer_agent_data_t *) gthis->data;
   return data->name;
}


//recursion to find the move
static int pick_col(board_t * b, player_t color, player_t original, int level, unsigned int max, unsigned int * move){
   //check if there is a winner or draw
   player_t player;
   int res = board_has_winner (b, &player);
   int size = board_get_height(b) * board_get_width(b);
   //get score if there is a winner
   if(res == 1){
      if (player == original){
         return size-level;  //lower level gets higher score when wins
      }
      else{
         return level-size; //high level gets better score when loses
      }
   }
   else if (res == -1){
      return 0;  //draw
   }
   
   int value; //score value
   unsigned int col=-1; //score optimal column choice
   //decide initial color
   if(color == original){
      value=INT_MIN;} 
   else{value=INT_MAX;}
   //find empty spot
   for (unsigned int i=0; i<max; ++i){
      //check if move is valid
      if(!board_can_play_move(b, color, i)){
         continue;
      }

      //play move
      board_play(b,i,color);

      // AI turn
      if(color == original){
         int tmp = pick_col(b,(color == PLAYER_BLUE? PLAYER_YELLOW:PLAYER_BLUE),original,level+1,max,move);
         if(value<tmp){ //get max score
            col = i;
            value = tmp;
         }
      }
      //opponent turn
      else{
         int tmp = pick_col(b,original,original,level+1,max,move);
         if(value>tmp){ //get min score
            value = tmp;
         }
      }
      // Remove token and try next pos
      board_unplay (b, i);
   }
   //update move
   *move = col;

   return value;
} 



//decide agent move
static int computer_agent_play (agent_t * gthis, const board_t * b, player_t color, unsigned int * move)
{
   //copy board
   board_t * tmp;
   board_duplicate(&tmp, b); 
   
   unsigned int max = board_get_width(b); //max width
   pick_col(tmp, color, color,0,  max, move); //call function

   //free board
   board_destroy(tmp);

   return 0;
}

//free void * data in agent_t
static bool computer_agent_destroy (agent_t * this)
{
   free (this->data);
   return true;
}

//create the computer agent
agent_t * computer_agent_create (){
    // Allocate agent_t structure
    agent_t * n = malloc (sizeof(agent_t));

    n->destroy = computer_agent_destroy;
    n->play = computer_agent_play;
    n->describe = computer_agent_describe;

    computer_agent_data_t * data = malloc (sizeof(computer_agent_data_t));
    n->data = data;

    //give a name
    data->name = "Computer";

    return n;
}

