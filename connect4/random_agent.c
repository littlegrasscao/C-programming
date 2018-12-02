#include "random_agent.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * In this file, put your implementation of a 'random agent', i.e. a player
 * who will make a VALID but random move.
 */

//store name
typedef struct
{
   char * name;
} random_agent_data_t;

//get the name of agent
static const char * random_agent_describe (agent_t * gthis)
{
   random_agent_data_t * data = (random_agent_data_t *) gthis->data;
   return data->name;
}

//decide agent move
static int random_agent_play (agent_t * gthis, const board_t * b, player_t color, unsigned int * move)
{
    const unsigned int width = board_get_width(b); //get width

    int column = rand()%width; //get a random column

    while(true){
        *move = column;
        //decide if col is valid
        if (!board_can_play_move(b, color, *move))
        {
            column = rand()%width; //reget the column is invalid
            continue;
        }

        return 0;
    }
}

//free void * data in agent_t
static bool random_agent_destroy (agent_t * this)
{
   free (this->data);
   return true;
}

//create the random agent
agent_t * random_agent_create (){
    // Allocate vtable structure
    agent_t * n = malloc (sizeof(agent_t));

    n->destroy = random_agent_destroy;
    n->play = random_agent_play;
    n->describe = random_agent_describe;

    random_agent_data_t * data = malloc (sizeof(random_agent_data_t));
    n->data = data;

    //give a name
    data->name = "Random";

    return n;
}

