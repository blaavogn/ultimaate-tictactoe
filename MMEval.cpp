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

			for(int i = 0; i < 81; i++){
				if(board[i] == 0){
					tmpBoard[i % 9]++;
				}
			}
			for(int i = 0; i < 9; i++){
				int lm = lmBoard[i];
				if(lm == DR){
					continue;
				}

				h += (lmBoard[i] == ND ? 0 : (lmBoard[i] == X ? 1 : -1)) * 200;
				uint64_t l_eval = ticTacEval->eval(board + i * 9);
				int plP = (l_eval & ticTacEval->BM_PL) ? 1 : 0;
				int poP = (l_eval & ticTacEval->BM_OP) ? 1 : 0;

				int sh = i * 2;
				int plCon = (eval >> (sh + 20)) & ticTacEval->BM_EVAL; 
				int opCon = -((eval >> (sh + 38)) & ticTacEval->BM_EVAL); 
				int t = tmpBoard[i % 9];
				h += plCon * t * plP + opCon * t * poP;
			}

			return h * (pl == X ? 1 : -1);
		};
};