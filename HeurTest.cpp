#include <iostream>
#include <vector>
#include "MMEval.cpp"
#include "Printer.cpp"

class HeurTest{		
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	int *moves;

	Printer printer;
	TicTacEval *ticTacEval;
	MMEval *mmEval; 
	
	public:
		HeurTest(std::vector<std::string> *inBoards){

			mmEval = new MMEval();
			ticTacEval = new TicTacEval();
			int boardCounts = inBoards->size() * 2;
			
			char* boards = new char[boardCounts * 81];
			uint64_t* lmBoard = new uint64_t[9];

			int count = 0;
			for(auto it = inBoards->begin(); it != inBoards->end(); ++it ){
				for(int i = 0; i < 81; i++){
					char v = (*it).at(i * 2);
					boards[count * 81 * 2 + i] = (v == '0' ? 0 : (v == '1' ? X : O));
					boards[count * 81 * 2 + 81 + i] = (v == '0' ? 0 : (v == '1' ? O : X));
				}
				count++;
			}
		
			for(int i = 0; i < boardCounts; i++){
				char* board = boards + 81 * i;
				for(int j = 0; j < 9; j++){
					lmBoard[j] = ticTacEval->eval(board + j * 9);
				}

				eval(board, lmBoard, i % 2);
				if(i % 2 == 1)
					printer.Print(board);
			}
		}

	private:
		void eval(char* boards, uint64_t* lmBoard, int flip){
			char pl1 = (flip == 0) ? 'O' : 'X';
			char pl2 = (flip == 0) ? 'X' : 'O';
			fprintf(stderr,"%c: %f, %c: %f\n", pl1,mmEval->H(boards, lmBoard, 1, ticTacEval), pl2,
																 mmEval->H(boards, lmBoard, 2, ticTacEval));
		}
};


int main(){
  std::string input_line;
	std::vector<std::string> *lines = new std::vector<std::string>();

	while (getline(std::cin, input_line)) {
  	lines->push_back(input_line);
  };
	new HeurTest(lines);

	return 0;
}