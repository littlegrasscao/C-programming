To run the game, download the folder and in the terminal type:
xxx@linux3:~/connect4$ make game
xxx@linux3:~/connect4$ ./game

The game requires user to enter height, width and run. If choose a Computer AI, recommend 4*4 board with 3 runs.(Take less time) 
Player Yellow is 1 and Player Blue is 2.
""""
Welcome to connect-four!
Please pick player yellow: (H)uman, (R)andom or (C)omputer? C
Please pick player blue: (H)uman, (R)andom or (C)omputer? H
Your name? sun
Board height [6]? 
4
Board width [7]? 
4
Board run (to win) [4]? 
3
Current board:
0 0 0 0 
0 0 0 0 
0 0 0 0 
0 0 0 0 
Computer(yellow) is next to play:
Computer(yellow) picked column 2
Current board:
0 0 0 0 
0 0 0 0 
0 0 0 0 
0 1 0 0 
sun(blue) is next to play:
Your move, sun (column?): 2
sun(blue) picked column 2
Current board:
0 0 0 0 
0 0 0 0 
0 2 0 0 
0 1 0 0 
""""
The game will continue until there is a winner or draw.




Computer AI agent implementation:

I used MinMax algorithm to create the AI. In each step, AI should be able to find the best move(column) by picking the best path in a decision tree where each node is a position on the board.
When it is AI's turn, AI tries to choose the best move that maximize the score. But in the opponent's turn, we assume opponent will try to maximize his score. Thus the minimum score will be chosen in the opponent's turn. 

In the recurssion:
1. Check in every level and position if there is a win or draw occurs. If yes, return a score. Score=100 for AI win, score=-100 for opponent win and score=0 for draw.
2. When it is AI's turn, pick the maximum score among all the possible positions and make a move for AI on that position.
3. When it is opponent's turn, pick the minimum score among all the possible positions and make a move for opponent on that position.

The recurssion steps as soon as there is a winner, so it is not necessary to fill the board every time.





To make the game last longer even if AI will lose is adding a level parameter. 
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

