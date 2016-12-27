#include <iostream>
#include "TicTacEval.cpp"

class MMEval{
	char *tmpBoard;
		
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	public:
		MMEval(){
			tmpBoard = (char*) malloc(sizeof(char) * 9);	
		}

		float H(char *board, char *lmBoard, char pl, TicTacEval *ticTacEval){
			float h = 0;
			uint64_t eval = ticTacEval->evalMacro(lmBoard);

			for(int i = 0; i < 9; i++){
				tmpBoard[i] = 0;
			}
			
			int free = 0;
			for(int i = 0; i < 9; i++){
				if(lmBoard[i] != ND){
					continue;
				}
				int in = i * 9;
				for(int j = 0; j < 9; j++){
					int v = board[in + j];
					if(v != 0){
						free += (v == X) ? 20 : -20;
					}else{
						tmpBoard[j] += 1;
					}
				}
			}

			h += free;

			int plBest = 0;
			int opBest = 0;

			for(int i = 0; i < 9; i++){
				int lm = lmBoard[i];
				if(lm == DR){
					continue;
				}

				int t = (lmBoard[i] == ND ? 0 : (lmBoard[i] == X ? 1 : -1)) * 15;
				h += t;
				
				int sh = i * 2;
				int plCon = (eval >> (sh + 2)) & ticTacEval->BM_EVAL; 
				int opCon = ((eval >> (sh + 22)) & ticTacEval->BM_EVAL); 

				plBest = std::max(plCon - 2, plBest);
				opBest = std::max(opCon - 2, opBest);
				t = plCon * 3 * tmpBoard[i] + opCon * -3 * tmpBoard[i];

				h += t;
			}

			h += plBest * 100 - opBest * 100;
			return h * (pl == X ? 1 : -1);
		};
};