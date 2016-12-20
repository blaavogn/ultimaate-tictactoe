#include <iostream>

class Engine{
	char *board;
	char *mBoard;
	char *sTmp;
	char *lTmp;
	int placeNext;
	char pl, op;
	int lMove;
	public:
		Engine();
		void Update(int*);
		void Print();
		std::pair<int, int> Move(int);
		void SetPlayer(char);
	private:
		char ValidateBig(char*);
		char ValidateSmall(char*);
		bool CanPlace(char*);
		float Max(int);
		float Min(int);
		int CTI(int, int);
};

Engine::Engine(){
	board     = (char*) malloc(sizeof(char) * 81);
	sTmp 		  = (char*) malloc(sizeof(char) * 9);
	lTmp 		  = (char*) malloc(sizeof(char) * 9);
	placeNext  = -1;

	for(int i = 0; i < 81; i++){
		board[i] = 0;
	}

	for(int i = 0; i < 9; i++){
		sTmp[i] = 0;
		lTmp[i] = 0;
	}
}

int Engine::CTI(int x, int y){
	return (y / 3) * 9 + (x / 3) * 27 + y % 3 + (x % 3) * 3;
}

void Engine::Update(int *inBoard){
	for(int y = 0; y < 9; y++){
		for(int x = 0; x < 9; x++){
			int index = this->CTI(x,y);
			int oldValue = board[index];
			int newValue = inBoard[x + y * 9];
			
			if(oldValue != newValue){
				board[index] = newValue;
				if(newValue != pl){
					placeNext = x;
				}
			}
		}
	}	
	this->Print();
}

void Engine::SetPlayer(char pl){
	this->pl = pl;
  op = (pl == 1) ? 2 : 1;  
  printf("Player is %d", pl);
}

std::pair<int, int> Engine::Move(int time){
	int macroIndex = placeNext * 9;
	int bestMove = -1;

	int minLim = 0, maxLim = 81;

	if(this->ValidateSmall(board + macroIndex) == 0){ //Strict macro placement
		minLim = macroIndex;
		maxLim = macroIndex + 9;
	}

	float max = -INFINITY;

	for(int i = minLim; i < maxLim; i++){
		if(i % 9 == 0 && this->ValidateSmall(board + i) != 0){
			i += 9;
			continue; //Skipping macros that is done
		} 

		if(board[i] == 0){
			board[i] = pl;
			float e = Min(i);
			max = (max > e) ? max : e;
			bestMove = i;
			board[i] = 0;
		}
	}

	//CONVERT MOVE BACK
	return std::make_pair(0, 0);	
}


float Max(int move){
	return -INFINITY;
}

float Min(int move){
	return +INFINITY;
}

bool Engine::CanPlace(char *b){
	bool emp = false;
	for(int i = 0; i < 9; i++){
		if(b[i] == 0){
			emp = true;
		}
	}
	return emp && this->ValidateSmall(b);
}

char Engine::ValidateBig(char *b){
	for(int i = 0; i < 9; i++){
		lTmp[i] = this->ValidateSmall(b + i * 9);
	}
	return this->ValidateSmall(lTmp);
}

char Engine::ValidateSmall(char *b){
	char l, v;
	//Horizontal
	for(char i = 0; i < 3; i++){
		l = b[i * 3];
		for(char j = 0; j < 3; j++){
			v = b[i * 3 + j];	
			if(l != v || v == 0){
				v = 0;
				break;
			}
			l = v;
		}
		if(v != 0){
			return v;
		}
	}
	//Vertical
	for(int i = 0; i < 3; i++){
		l = b[i];
		for(char j = 0; j < 3; j++){
			v = b[i + j * 3];
			if(l != v || v == 0){
				v = 0;
				break;
			}
			l = v;
		}
		if(v != 0){
			return v;
		}
	}
	
	l = b[0];
	for(char i = 0; i < 3; i++){
		v = b[i + i * 3];
		if(l != v || v == 0){
			v = 0;
			break;
		}
		l = v;
	}
	if(v != 0){
		return v;
	}

	l = b[2];
	for(char i = 0; i < 3; i++){
		v = b[2 - i + i * 3]; //b[2 + i * 2]???
		if(l != v || v == 0){
			v = 0;
			break;
		}
		l = v;
	}
	
	if(v != 0){
		return v;
	}

	for(int i = 0; i < 9; i++){
		if(b[i] == 0){
			return 0;
		}
	}

	return -1;
}

void Engine::Print(){
	for(char k = 0; k < 9; k++){
		if(k%3 == 0){
			printf("-------------\n");
		}
		for(char i = 0; i < 3; i++){
			for(char j = 0; j < 3; j++){
				printf ("%d", board[(k / 3) * 18 + k * 3 + i * 9 + j]);
			}
			printf("|");
		}
		printf("\n");
	}
	printf ("-------------\n");
}

