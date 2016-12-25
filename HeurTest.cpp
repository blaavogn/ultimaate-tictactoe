#include <iostream>
#include "MMEval.cpp"
#include "Printer.cpp"

class HeurTest{		
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	Printer printer;
	TicTacEval *ticTacEval;
	MMEval *mmEval; 
	public:
		HeurTest(){
			mmEval = new MMEval();
			ticTacEval = new TicTacEval();
			char boardNum = 3;
			
			char** boards = new char*[boardNum];
			char* lmBoard = new char[9];

			for(int i = 0; i < boardNum * 2; i++){
				for(int j = 0; j < 81; j++)
					boards[i * 81 + j] = 0;
			}

			for(int i = 0; i < boardNum * 2; i++){
				for(int j = 0; j < 9; j++){
					char* a = boards[i + j * 9];
					uint64_t yy = ((ticTacEval->eval(a)) & (ticTacEval->BM_EVAL));
					lmBoard[0] = 1; //	Den her linje
				}
			}	
		}

	private:
		void flip(char* board){
			for(int i = 0; i < 81; i++){
				char v = board[i];
				if(v != 0){
					board[i] = (v == X ? O : X);
				}
			}
		}
		void eval(char* boards, char* lmBoard){
			printf("mmEval; %f, %f\n", mmEval->H(boards, lmBoard, 1, ticTacEval), 
																 mmEval->H(boards, lmBoard, 2, ticTacEval));
		}

		void Update(char *inBoard, char* outBoard){
			for(int y = 0; y < 9; y++){
				for(int x = 0; x < 9; x++){
					int index = this->CTI(x,y);
					int pV = inBoard[x + y * 9];
					int newValue = (pV == 0) ? ND : ((pV == 1) ? X : O);
					outBoard[index] = newValue;	
				}
			}	
		}

		inline int CTI(int x, int y){
			int macroX = x / 3; 
			int macroY = (y / 3) * 3;
			int macro = macroX + macroY;

			int microX = x % 3; 
			int microY = y % 3 * 3; 
			int micro = microX + microY;
			return micro + macro * 9; 
		}
};


int main(){
	HeurTest* h = new HeurTest();

	return 0;
}