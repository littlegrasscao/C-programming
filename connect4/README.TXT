
Computer AI agent implementation:

I used MinMax algorithm to create the AI. In each step, AI should be able to find the best move(column) by picking the best path in a decision tree where each node is a position on the board.
When it is AI's turn, AI tries to choose the best move that maximize the score. But in the opponent's turn, we assume opponent will try to maximize his score. Thus the minimum score will be chosen in the opponent's turn. 

In the recurssion:
1. Check in every level and position if there is a win or draw occurs. If yes, return a score. Score=100 for AI win, score=-100 for opponent win and score=0 for draw.
2. When it is AI's turn, pick the maximum score among all the possible positions and make a move for AI on that position.
3. When it is opponent's turn, pick the minimum score among all the possible positions and make a move for opponent on that position.

The recurssion steps as soon as there is a winner, so it is not necessary to fill the board every time.





**I did Step 5.
To make the game last longer even if AI will lose is add a level parameter. 
   int size = board_get_height(b) * board_get_width(b);
   //check for winning
   if(res == 1){
      if (player == AI){
         return size-level;
      }
      else{
         return level-size;
      }
   }
If AI wins, the lower level, the higher score. If AI loses, the higher level, the higher score.

