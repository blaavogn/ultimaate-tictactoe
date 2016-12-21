#include "Engine.cpp"
#include <iostream>

void place(int *b, int x, int y, int v){
	b[x + y*9] = v;
}

int main(int argc, char *argv[]) {
  Engine* eng = new Engine();
 	
 	int *board = (int*) malloc(sizeof(int) * 81);
 	for(int i = 0; i < 81; i++){
 		board[i] = 0;
 	}


 	int op = (int) (argv[1][0] - '0');
 	int pl = (op == 1) ? 2 : 1;
 	eng->SetPlayer(op);

 	eng->Update(board);

	char *buffer;
  size_t n = 12;
  buffer = (char*) malloc(n);

  int turn = (pl == 1) ? 1 : 2;

 	while(true){ 		
 		if(turn == 1){ //player turn
 			if(!getline(&buffer, &n, stdin)){ printf("Error.\n");}
 			int x = atoi(buffer);
 			if(!getline(&buffer, &n, stdin)){ printf("Error.\n");}
 			int y = atoi(buffer);

 			board[x + y * 9] = pl;
 			eng->Update(board);

 		}else{
 			std::pair<int, int> move = eng->Move(500);

 			board[move.first + move.second * 9] = op;
 			eng->Update(board);
 		}
 		turn = (turn == 1) ? 2 : 1;
 	}

  return 0;
}
