
#include "player_agent.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

static char * mystrdup (const char * s)
{
   char * n = (char *) malloc (strlen(s)+1);
   strcpy (n, s);
   return n;
}

typedef struct
{
   char * name;
} player_agent_data_t;

static
const char * player_agent_describe (agent_t * gthis)
{
   player_agent_data_t * data = (player_agent_data_t *) gthis->data;
   return data->name;
}


static
int player_agent_play (agent_t * gthis, const board_t * b,
        player_t color, unsigned int * move)
{
   const unsigned int width = board_get_width(b);

   player_agent_data_t * data = (player_agent_data_t *) gthis->data;

   while (true)
   {
      printf ("Your move, %s (column?): ", data->name);
      char buf[256];
      if (!fgets (buf, sizeof(buf), stdin))
         return -1;

      errno=0;
      int column = strtol(buf, NULL, 10);
      if (errno || (column < 1 || column > width))
      {
         printf ("Invalid column! Try again.\n");
         continue;
      }

      *move = column-1;

      if (!board_can_play_move(b, color, *move))
      {
         printf ("Invalid move! Try again.\n");
         continue;
      }

      return 0;
   }
}

static
bool player_agent_destroy (agent_t * this)
{
   player_agent_data_t * data = (player_agent_data_t *) this->data;
   free (data->name);
   free (this->data);
   return true;
}

agent_t * player_agent_create (const char * name)
{
    // Allocate vtable structure
    agent_t * n = malloc (sizeof(agent_t));

    n->destroy = player_agent_destroy;
    n->play = player_agent_play;
    n->describe = player_agent_describe;

    player_agent_data_t * data = malloc (sizeof(player_agent_data_t));
    n->data = data;

    data->name = mystrdup(name);

    return n;
}

