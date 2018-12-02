#include "board.h"

#include <CUnit/Basic.h>

#include <stdio.h>
#include <stdbool.h>

static void test_board_get_xxx ()
{
   board_t * newb;
   CU_ASSERT_FATAL(board_create (&newb, 10, 20, 6, NULL));
   CU_ASSERT_PTR_NOT_NULL_FATAL(newb);
   CU_ASSERT_EQUAL(board_get_height(newb), 10);
   CU_ASSERT_EQUAL(board_get_width(newb), 20);
   CU_ASSERT_EQUAL(board_get_run(newb), 6);
   CU_ASSERT(board_destroy(newb));
}

static inline void check_pos (const board_t * b, unsigned int r, unsigned int c, player_t expected)
{
   player_t token;
   CU_ASSERT(board_get(b, r, c, &token));
   CU_ASSERT_EQUAL(token, expected);
}

static void test_board_clear()
{
   // Create with init
   player_t init[] = {PLAYER_EMPTY, PLAYER_YELLOW, PLAYER_BLUE,
                      PLAYER_EMPTY, PLAYER_BLUE, PLAYER_YELLOW,
                      PLAYER_EMPTY, PLAYER_BLUE, PLAYER_EMPTY,
                      PLAYER_EMPTY, PLAYER_BLUE, PLAYER_EMPTY};

   board_t * newb;
   CU_ASSERT_FATAL(board_create (&newb, 4, 3, 4, init));
   CU_ASSERT_PTR_NOT_NULL_FATAL(newb);

   for (unsigned int round=0; round<10; ++round)
   {
      CU_ASSERT(board_clear(newb));

      for (unsigned int c=0; c<board_get_width(newb); ++c)
      {
         for (unsigned int r=0; r<board_get_height(newb); ++r)
         {
            player_t p = PLAYER_YELLOW;
            CU_ASSERT(board_get(newb, r, c, &p));
            CU_ASSERT_EQUAL(p, PLAYER_EMPTY);
         }
      }
      
      // now fill board
      for (unsigned int c=0; c<board_get_width(newb); ++c)
      {
         for (unsigned int r=0; r<board_get_height(newb); ++r)
         {
            player_t p;
            CU_ASSERT(board_get(newb, r, c, &p));
            CU_ASSERT_EQUAL(p, PLAYER_EMPTY);
            p = ((r+c) & 0x01 ? PLAYER_YELLOW : PLAYER_BLUE);
            CU_ASSERT(board_play(newb, c, p));
         }
      }
   }


   CU_ASSERT(board_destroy(newb));
}

static void test_create_destroy ()
{
   board_t * newb;

   // Create without values
   CU_ASSERT_FATAL(board_create (&newb, 10, 20, 6, NULL));
   CU_ASSERT_PTR_NOT_NULL_FATAL(newb);
   CU_ASSERT(board_destroy(newb));

   // Create with init
   player_t init[] = {PLAYER_EMPTY, PLAYER_YELLOW, PLAYER_BLUE,
                      PLAYER_EMPTY, PLAYER_BLUE, PLAYER_YELLOW,
                      PLAYER_EMPTY, PLAYER_BLUE, PLAYER_EMPTY,
                      PLAYER_EMPTY, PLAYER_BLUE, PLAYER_EMPTY};

   CU_ASSERT_FATAL(board_create (&newb, 4, 3, 4, init));
   CU_ASSERT_PTR_NOT_NULL_FATAL(newb);

   // Check position
   check_pos (newb, 0, 0, PLAYER_EMPTY);
   check_pos (newb, 0, 0, PLAYER_EMPTY);
   check_pos (newb, 0, 1, PLAYER_YELLOW);
   check_pos (newb, 0, 2, PLAYER_BLUE);
   check_pos (newb, 1, 0, PLAYER_EMPTY);
   check_pos (newb, 1, 1, PLAYER_BLUE);
   check_pos (newb, 1, 2, PLAYER_YELLOW);
   check_pos (newb, 2, 0, PLAYER_EMPTY);
   check_pos (newb, 2, 1, PLAYER_BLUE);
   check_pos (newb, 2, 2, PLAYER_EMPTY);
   check_pos (newb, 3, 0, PLAYER_EMPTY);
   check_pos (newb, 3, 1, PLAYER_BLUE);
   check_pos (newb, 3, 2, PLAYER_EMPTY);

   // Check clear
   CU_ASSERT(board_clear(newb));
   for (unsigned int r=0; r<board_get_height(newb); ++r)
   {
      for (unsigned int c=0; c<board_get_width(newb); ++c)
      {
         player_t p = PLAYER_YELLOW;
         CU_ASSERT(board_get(newb, r, c, &p));
         CU_ASSERT_EQUAL(p, PLAYER_EMPTY);
      }
   }

   CU_ASSERT(board_destroy(newb));
}


static void test_board_get ()
{
    board_t * board;
    CU_ASSERT_FATAL(board_create (&board, 3, 4, 5, NULL));
    CU_ASSERT_PTR_NOT_NULL_FATAL(board);

    player_t t;
    CU_ASSERT_FALSE(board_get(board, 3, 0, &t));
    CU_ASSERT_FALSE(board_get(board, 0, 4, &t));
    CU_ASSERT_TRUE(board_get(board, 2, 0, &t));
    CU_ASSERT_TRUE(board_get(board, 0, 3, &t));
    CU_ASSERT_TRUE(board_get(board, 2, 3, &t));
    CU_ASSERT(board_destroy(board));
}

static void test_board_print ()
{
   FILE * f = fopen ("/dev/null", "w");
   CU_ASSERT_PTR_NOT_NULL_FATAL(f);

   board_t * board;
   CU_ASSERT_FATAL(board_create (&board, 3, 4, 5, NULL));
   CU_ASSERT_PTR_NOT_NULL_FATAL(board);
   CU_ASSERT(board_print (board, f));
   CU_ASSERT(board_destroy(board));
   fclose(f);
}

static void test_board_duplicate ()
{
   // Create with init
   player_t init[] = {PLAYER_EMPTY, PLAYER_YELLOW, PLAYER_BLUE,
                      PLAYER_EMPTY, PLAYER_BLUE, PLAYER_YELLOW,
                      PLAYER_EMPTY, PLAYER_BLUE, PLAYER_EMPTY,
                      PLAYER_EMPTY, PLAYER_BLUE, PLAYER_EMPTY};

   board_t * newb;
   CU_ASSERT_FATAL(board_create (&newb, 4, 3, 4, init));
   CU_ASSERT_PTR_NOT_NULL_FATAL(newb);

   board_t * newb2;
   CU_ASSERT_FATAL(board_duplicate(&newb2, newb));

   CU_ASSERT_EQUAL(board_get_height(newb), board_get_height(newb2));
   CU_ASSERT_EQUAL(board_get_width(newb), board_get_width(newb2));
   CU_ASSERT_EQUAL(board_get_run(newb), board_get_run(newb2));
   
   unsigned int width = board_get_width(newb);
   unsigned int height = board_get_height(newb);

   player_t old[height][width];

   for (unsigned int r=0; r<board_get_height(newb); ++r)
   {
      for (unsigned int c=0; c<board_get_width(newb); ++c)
      {
         player_t b1,b2;
         CU_ASSERT(board_get(newb, r, c, &b1));
         CU_ASSERT(board_get(newb2, r, c, &b2));
         CU_ASSERT_EQUAL(b1, b2);
         old[r][c]=b1;
      }
   }

   CU_ASSERT(board_clear(newb));

   for (unsigned int r=0; r<board_get_height(newb); ++r)
   {
      for (unsigned int c=0; c<board_get_width(newb); ++c)
      {
         player_t b2;
         CU_ASSERT(board_get(newb2, r, c, &b2));
         CU_ASSERT_EQUAL(b2, old[r][c]);
      }
   }
 
   CU_ASSERT(board_destroy(newb2));
   CU_ASSERT(board_destroy(newb));
}

void test_board_play ()
{
   board_t * newb;
   CU_ASSERT_FATAL(board_create (&newb, 4, 3, 4, NULL));
   CU_ASSERT_PTR_NOT_NULL_FATAL(newb);

   // Unplay should fail if there is no token at the position
   for (unsigned int i=0; i<board_get_width(newb); ++i)
   {
      CU_ASSERT_FALSE(board_unplay(newb, i));
   }

   // Try playing one move
   CU_ASSERT(board_play(newb, 2, PLAYER_YELLOW));
   player_t t;
   CU_ASSERT(board_get(newb, 0, 2, &t));
   CU_ASSERT_EQUAL(t, PLAYER_YELLOW);

   // Unplay should fail if there is no token at the position
   for (unsigned int i=0; i<board_get_width(newb); ++i)
   {
      if (i==2)
         CU_ASSERT(board_unplay(newb, i))
      else
         CU_ASSERT_FALSE(board_unplay(newb, i));

      CU_ASSERT(board_can_play(newb, PLAYER_YELLOW));
      CU_ASSERT(board_can_play(newb, PLAYER_BLUE));
   }

   for (unsigned int i=0; i<5; ++i)
   {
      // Try playing one move
      if (i<board_get_height(newb))
      {
         CU_ASSERT(board_can_play_move(newb, PLAYER_BLUE, 1));
         CU_ASSERT(board_play(newb, 1, PLAYER_BLUE));
         player_t t;
         CU_ASSERT(board_get(newb, i, 1, &t));
         CU_ASSERT_EQUAL(t, PLAYER_BLUE);
      }
      else
      {
         CU_ASSERT_FALSE(board_can_play_move(newb, PLAYER_BLUE, 1));
         CU_ASSERT_FALSE(board_play(newb, 1, PLAYER_BLUE));
      }

      // Board is not full so we should be able to play
      CU_ASSERT(board_can_play(newb, PLAYER_YELLOW));
      CU_ASSERT(board_can_play(newb, PLAYER_BLUE));
   }

   CU_ASSERT(board_destroy(newb));
}

void test_board_canplay()
{
   board_t * newb;
   CU_ASSERT_FATAL(board_create (&newb, 4, 3, 9, NULL));
   CU_ASSERT_PTR_NOT_NULL_FATAL(newb);

   for (unsigned int r=0; r<board_get_height(newb); ++r)
   {
      for (unsigned int c=0; c<board_get_width(newb); ++c)
      {
         player_t p = ((r+c) & 0x1 ? PLAYER_YELLOW : PLAYER_BLUE);
         CU_ASSERT(board_can_play(newb, PLAYER_YELLOW));
         CU_ASSERT(board_can_play(newb, PLAYER_BLUE));
         CU_ASSERT(board_can_play_move(newb, PLAYER_YELLOW, c));
         CU_ASSERT(board_can_play_move(newb, PLAYER_BLUE, c));
         CU_ASSERT(board_play(newb, c, p));

         player_t g;
         CU_ASSERT(board_get(newb, r, c, &g));
         CU_ASSERT_EQUAL(g, p);
      }
   }

   // now board is full
   CU_ASSERT_FALSE(board_can_play(newb, PLAYER_YELLOW));
   CU_ASSERT_FALSE(board_can_play(newb, PLAYER_BLUE));
   for (unsigned int c=0; c<board_get_width(newb); ++c)
   {
      CU_ASSERT_FALSE(board_can_play_move(newb, PLAYER_YELLOW, c));
      CU_ASSERT_FALSE(board_can_play_move(newb, PLAYER_BLUE, c));
   }

   CU_ASSERT(board_destroy(newb));
}



/*
void test_winner (const player_t * i, int ret, player_t win)
{
    board_t * board;
    board_create (&board, i, 3, 4, 5);

    board_print (board, stdout);

    player_t winner;
    int thisret = board_has_winner (board, &winner);
    CU_ASSERT(ret == thisret);
    CU_ASSERT(!ret || (winner == win));

    printf ("Winner: %i, %s\n\n", ret, (winner == PLAYER_YELLOW ?
                "yellow" : "blue"));
}

void test_checkwin()
{
    player_t y = PLAYER_YELLOW;
    player_t b = PLAYER_BLUE;
    player_t e = PLAYER_EMPTY;

    assert (e == 0);

    {
    player_t i[] = { 0, 0, y, y, y, y, 
                     0, 0, 0, 0, 0, 0, 
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0 };
    test_winner(i, 1, PLAYER_YELLOW);
    }

    {
    player_t i[] = { 0, 0, y, b, y, y, 
                     0, 0, 0, 0, 0, 0, 
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0 };
    test_winner(i, 0, PLAYER_YELLOW);
    }

    {
    player_t i[] = { 0, 0, y, b, y, y, 
                     0, 0, y, 0, 0, 0, 
                     0, 0, y, 0, 0, 0,  
                     0, 0, y, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0 };
    test_winner(i, 1, PLAYER_YELLOW);
    }

    {
    player_t i[] = { 0, 0, 0, y, 0, b, 
                     0, 0, 0, y, 0, b, 
                     0, 0, 0, y, 0, b,  
                     0, 0, 0, y, 0, y,  
                     0, 0, 0, 0, 0, b,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0 };
    test_winner(i, 1, PLAYER_YELLOW);
    }


    {
    player_t i[] = { y, y, y, b, y, y, 
                     b, y, b, 0, 0, 0, 
                     y, b, y, 0, 0, 0,  
                     b, y, y, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0 };
    test_winner(i, 1, PLAYER_BLUE);
    }

    {
    player_t i[] = { y, y, y, b, y, y, 
                     b, y, b, b, b, b, 
                     y, b, y, 0, 0, 0,  
                     y, y, y, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0 };
    test_winner(i, 1, PLAYER_BLUE);
    }

    {
    player_t i[] = { y, y, y, b, y, y, 
                     b, y, b, y, b, b, 
                     y, b, y, b, y, b,  
                     y, y, y, b, b, y,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0,  
                     0, 0, 0, 0, 0, 0 };
    test_winner(i, 1, PLAYER_YELLOW);
    }
}

*/

void test_board_winner_no()
{
   // Dataset from http://archive.ics.uci.edu/ml/machine-learning-databases/connect-4/

   FILE * f = fopen("connect-4.data", "r");
   CU_ASSERT_PTR_NOT_NULL_FATAL(f);

   board_t * b;
   int r_h, r_w, r_r;

   unsigned long int test_number = 0;

   do
   {
      if (fscanf(f, "%i,%i,%i,", &r_h, &r_w, &r_r) == EOF)
      {
         if (ferror(f))
         {
            fprintf(stderr, "Error reading file!\n");
            CU_ASSERT_FATAL(false);
         }
         break;
      }

      CU_ASSERT_FATAL(board_create(&b, r_h, r_w, r_r, NULL));

      unsigned int width=board_get_width(b);
      unsigned int height=board_get_height(b);

      char line[255];
      // read rest of line
      if (!fgets(line, sizeof(line), f))
      {
         fprintf(stderr, "Error reading file!");
         CU_ASSERT_FATAL(false);
      }
      const char * p = line;

      for (unsigned int c=0; c<width; ++c)
      {
         for (unsigned int r=0; r<height; ++r)
         {
            switch (*p++)
            {
               case 'b':
                  break;
               case 'x':
                  CU_ASSERT_FATAL(board_play(b, c, PLAYER_YELLOW));
                  break;
               case 'o':
                  CU_ASSERT_FATAL(board_play(b, c, PLAYER_BLUE));
                  break;
               default:
                  CU_FAIL_FATAL("Error parsing data file");
            }
            char n = *p++;
            CU_ASSERT_EQUAL_FATAL(n, ',');
         }
      }

      // Expect p to be 'draw', 'win' or 'loss'
      int expected;
      player_t expected_player = PLAYER_EMPTY;
      if (!strcmp(p, "draw\n"))
      {
         expected = -1;
      }
      else if (!strcmp(p, "loss\n"))
      {
         expected = 1;
         expected_player=PLAYER_BLUE;
      }
      else if (!strcmp(p, "win\n"))
      {
         expected = 1;
         expected_player=PLAYER_YELLOW;
      }
      else if (!strcmp(p, "?\n"))
      {
         expected = 0;
      }
      else
      {
         CU_FAIL_FATAL("Error parsing win/loss/draw");
         // silence compiler warning about expected being used uninitialized.
         return;
      }

      player_t player;
      int result = board_has_winner(b, &player);
      if (result != expected)
      {
         fprintf (stderr, "Error in board_has_winner: following board"
               " should be: %i but was %i (test board #%lu)\n", expected,
               result, test_number);
         CU_ASSERT(board_print(b, stderr));
         fputs(line, stderr);
         fputs("\n", stderr);
      }
      CU_ASSERT_EQUAL(result, expected);

      // If there was a winner, check that we have the right winner
      if (expected == 1)
      {
         if (player != expected_player)
         {
            fprintf (stderr, "Error in board_has_winner: following board"
                  " should be won by %i but was won by %i (test board #%lu)\n",
                  expected_player, player, test_number);
            CU_ASSERT(board_print(b, stderr));
            fputs(line, stderr);
            fputs("\n", stderr);
         }
         CU_ASSERT_EQUAL(player, expected_player);
      }

      CU_ASSERT_FATAL(board_clear(b));

      CU_ASSERT(board_destroy(b));

      ++test_number;

   } while(1);

   fclose(f);
}

int init_suite1 ()
{
    return 0;
}

int clean_suite1 ()
{
    return 0;
}

int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("board", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if ((NULL == CU_add_test(pSuite, "create_destroy", test_create_destroy))
    || (NULL == CU_add_test(pSuite, "board_get_xxx", test_board_get_xxx))
    || (NULL == CU_add_test(pSuite, "board_get", test_board_get))
    || (NULL == CU_add_test(pSuite, "board_clear", test_board_clear))
    || (NULL == CU_add_test(pSuite, "board_play_unplay", test_board_play))
    || (NULL == CU_add_test(pSuite, "board_print", test_board_print))
    || (NULL == CU_add_test(pSuite, "board_duplicate", test_board_duplicate))
    || (NULL == CU_add_test(pSuite, "board_can_play", test_board_canplay))
    || (NULL == CU_add_test(pSuite, "board_has_winner_no", test_board_winner_no))
       )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}


