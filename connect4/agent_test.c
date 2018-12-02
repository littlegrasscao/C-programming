#include <CUnit/Basic.h>
#include "agent_test.h"
#include "board.h"

// Function the tests will use to create the agent
static agent_t * (*create_agent)() = NULL;

/// Create a 4x4-3 board
static inline board_t * create_board()
{
   board_t * newb;
   CU_ASSERT_FATAL(board_create (&newb, 3, 4, 4, NULL));
   CU_ASSERT_PTR_NOT_NULL_FATAL(newb);
   return newb;
 }

static void destroy_board(board_t * b)
{
   CU_ASSERT(board_destroy(b));
}


void set_create(agent_t * (*createfunc)())
{
   create_agent = createfunc;
}

// Simply test if we can create the agent and destroy it
static void test_create_destroy()
{
   agent_t * agents[32];
   const unsigned int count = sizeof(agents)/sizeof(agents[0]); 
   
   for (unsigned int i=0; i<count; ++i)
   {
      agents[i] = create_agent();
      CU_ASSERT_PTR_NOT_NULL_FATAL(agents[i]);
   }

   for (unsigned int i=0; i<count; ++i)
   {
      CU_ASSERT(agent_destroy(agents[i]));
   }
}

static void test_describe()
{
   agent_t * agents[32];
   const unsigned int count = sizeof(agents)/sizeof(agents[0]); 
   
   for (unsigned int i=0; i<count; ++i)
   {
      agents[i] = create_agent();
      CU_ASSERT_PTR_NOT_NULL_FATAL(agents[i]);
   }

   for (unsigned int i=0; i<count; ++i)
   {
      const char * ptr = agent_describe (agents[i]);
      CU_ASSERT_PTR_NOT_NULL(ptr);
      CU_ASSERT_NOT_EQUAL(strlen(ptr), 0);
   }

   for (unsigned int i=0; i<count; ++i)
   {
      CU_ASSERT(agent_destroy(agents[i]));
   }
}

typedef struct
{
   board_t * b;
   agent_t * a;
   player_t  color;
   bool      active;
} Game;

static void test_play()
{
   // Create a small board and expect valid moves
   Game games[3];
   const unsigned int count = sizeof(games)/sizeof(games[0]);

   for (unsigned int q=0; q<count; ++q)
   {
      games[q].b = create_board();
      games[q].a = create_agent();
      games[q].active = true;
      games[q].color = PLAYER_YELLOW;
      CU_ASSERT_PTR_NOT_NULL(games[q].b);
      CU_ASSERT_PTR_NOT_NULL(games[q].a);
   }

   unsigned int done = 0;
   while (done != count)
   {
      // Pick a game at random
      unsigned int q = rand() % count;
      while (!games[q].active)
      {
         ++q;
         if (q >= count)
            q=0;
      }

      unsigned int move = 99999;
      int ret = agent_play (games[q].a, games[q].b, games[q].color, &move); 

      // Check if it was possible to make a move
      if (!board_can_play(games[q].b, games[q].color))
      {
         games[q].active = false;
         ++done;
         CU_ASSERT_EQUAL(ret, -1);
         continue;
      }

      // Agent should have made a valid move
      CU_ASSERT_EQUAL(ret, 0);
      CU_ASSERT_TRUE(move >= 0 && move < board_get_width(games[q].b));

      CU_ASSERT(board_can_play_move(games[q].b, games[q].color, move));

      // Make the move
      CU_ASSERT(board_play (games[q].b, move, games[q].color));

      // Check if the game is over
      player_t player;
      int winner = board_has_winner (games[q].b, &player);

      if (winner != 0)
      {
         // game is over
         games[q].active = false;
         ++done;
         continue;
      }

      games[q].color = (games[q].color == PLAYER_YELLOW ?
                               PLAYER_BLUE : PLAYER_YELLOW);
   }

   for (unsigned int q=0; q<count; ++q)
   {
      destroy_board(games[q].b);
      CU_ASSERT(agent_destroy(games[q].a));
   }
}

bool add_agent_tests(CU_pSuite suite)
{
   if ((NULL == CU_add_test(suite, "agent_create", test_create_destroy))
    || (NULL == CU_add_test(suite, "agent_describe", test_describe))
    || (NULL == CU_add_test(suite, "agent_play", test_play)))
      return false;

   return true;
}

