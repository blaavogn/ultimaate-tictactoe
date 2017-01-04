#include <iostream>
#include "TicTacEval.cpp"

class MMEval{
	char *won; 

	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	const int H_WON = 50;
	public:
		MMEval(){
			won = new char[9];
		}

		float H(char *board, uint64_t *mBoardFull, char pl, TicTacEval *ticTacEval){
			float h = 0;

			for(int i = 0; i < 9; i++){
				int l_won = (mBoardFull[i] & TicTacEval::BM_EVAL);
				won[i] = l_won;
				h += toH(l_won) * H_WON;
			}
			
			uint64_t macroEvalPl = ticTacEval->evalPl(won, 2);
			uint64_t macroEvalOp = ticTacEval->evalPl(won, 1);

			for(int i = 0; i < 9; i++){
				if(won[i] != ND){
					continue;
				}	
				int pl = ((macroEvalPl >> (TicTacEval::plb + i * TicTacEval::shft)) & TicTacEval::BM_EVAL) * 
				          ((mBoardFull[i] >> (TicTacEval::plb + 18)) & TicTacEval::BM_EVAL);
				int op = ((macroEvalOp >> (TicTacEval::opb + i * TicTacEval::shft)) & TicTacEval::BM_EVAL) * 
				          ((mBoardFull[i] >> (TicTacEval::opb + 18)) & TicTacEval::BM_EVAL);
				h += pl * pl - op * op;
			}
			return h * (pl == X ? 1 : -1);
		}

		int toH(int p){
			return (p == 1 ? 1 : (p == 2 ? -1 : 0));
			// return p * -2 + 3;
		}
};
