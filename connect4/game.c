// DO NOT MODIFY THIS FILE
//
#include "board.h"
#include "player_agent.h"
#include "computer_agent.h"
#include "random_agent.h"
#include "agent.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>   // for time()
#include <errno.h>
#include <assert.h>


const char * player2string (player_t b)
{
   return (b == PLAYER_YELLOW ? "yellow" : "blue");
}

agent_t * new_random_player ()
{
   return random_agent_create ();
}

agent_t * new_computer_player ()
{
   return computer_agent_create();
}

agent_t * new_human_player ()
{
   char buf[128];
   printf ("Your name? ");
   if (!fgets (buf, sizeof(buf), stdin))
      exit(1);

   // Remove '\n'
   buf[strlen(buf)-1]=0;

   return player_agent_create(buf);
}


agent_t * select_player (player_t b)
{
   printf ("Please pick player %s: (H)uman, (R)andom or"
         " (C)omputer? ", player2string(b));
   while (true)
   {
      char buf[100];
      if (!fgets(buf, sizeof(buf), stdin))
         exit (1);

      if (!strcmp(buf, "H\n"))
         return new_human_player();
      else if (!strcmp(buf, "C\n"))
         return new_computer_player();
      else if (!strcmp(buf, "R\n"))
         return new_random_player();
      else
         printf ("Please pick C or H!\n");
   }
}

static int next_player (int cur)
{
   return (cur == 0 ? 1 : 0);
}

static unsigned int readnum (const char * prompt, unsigned int def)
{
   do
   {
      puts(prompt);
      char buf[255];
      if (!fgets(buf, sizeof(buf), stdin))
      {
         return def;
      }
      if (buf[0]=='\n' || !strlen(buf))
         return def;

      errno=0;
      unsigned int ret = strtol(buf, NULL, 10);
      if (errno)
      {
         printf("Not a valid number! Try again!\n");
         continue;
      }
      return ret;
   } while (true);
}

static board_t * create_board()
{
   unsigned int height = readnum ("Board height [6]? ", 6);
   unsigned int width = readnum ("Board width [7]? ", 7);
   unsigned int run = readnum ("Board run (to win) [4]? ", 4);
   
   board_t * b;
   board_create(&b, height, width, run, NULL);
   return b;
}

int main ()
{
   printf ("Welcome to connect-four!\n");

   player_t colors[] = {PLAYER_YELLOW, PLAYER_BLUE};
   agent_t * agents[sizeof(colors)/sizeof(colors[0])];

   agents[0] = select_player(colors[0]);
   agents[1] = select_player(colors[1]);

   // See manpage for srand
   srand(time(NULL));

   // Pick starting player
   int current_player = 0; //rand() & 0x1;

   // Create board
   board_t * b = create_board();


   int winner;

   while(true)
   {
      printf ("Current board:\n");
      board_print (b, stdout);

      agent_t * curagent = agents[current_player];
      player_t curcolor = colors[current_player];
      const char * agentname = agent_describe(curagent);
      const char * colorname = player2string(curcolor);

      printf ("%s(%s) is next to play:\n", agentname, colorname);

      // Check if there is a winner
      player_t winnercolor;
      int result = board_has_winner (b, &winnercolor);

      winner = (winnercolor == colors[0] ? 0 : 1);

      bool canplay = true;
      switch (result)
      {
         case -1:
            printf ("Game is a tie!\n");
            canplay = false;
            break;
         case 1:
            printf ("%s(%s) won the game!\n",
                  agent_describe(agents[winner]),
                  player2string(colors[winner]));
            canplay = false;
            break;
      }

      if (!canplay)
      {
         break;
      }

      // Check if we can play
      if (!board_can_play(b, curcolor))
      {
         printf ("No valid move can be made...\n");
         // but also there was on tie or no winner; must be an error!
         assert(false && "Should not happen!");
         break;
      }


      unsigned int move;
      int agent_status = agent_play (curagent, b, curcolor, &move);
      if (agent_status == -1)
      {
         // Agent can't decide on a move; loses game
         printf ("%s(%s) failed to decide on a move! Game lost!\n",
               agentname, colorname);
         winner = next_player(current_player);
         break;
      }

      // Agent wants to do move 'move'
      if (!board_play (b, move, curcolor))
      {
         printf ("%s(%s) failed to play a valid move! Game lost!\n",
               agentname, colorname);
         winner = next_player(current_player);
         break;
      }

      printf ("%s(%s) picked column %i\n", agentname, colorname, move+1);


      // Check if there is a winner

      current_player = next_player(current_player);
   }
   
   printf ("Final board:\n");
   board_print (b, stdout);

   agent_destroy(agents[0]);
   agent_destroy(agents[1]);

   board_destroy (b);

   return EXIT_SUCCESS;
}
