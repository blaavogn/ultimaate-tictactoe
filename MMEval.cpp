#include <iostream>
#include "TicTacEval.cpp"

class MMEval{
	char *won; 

	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	const int H_WON = 40;

	float H_MACRO[9] = {
		1.2, 1.0, 1.2,
		1.0, 3.0, 1.0,
		1.2, 1.0, 1.2
	};

	float H_MICRO[9] = {
		1.2, 2.5, 1.2,
		2.5, 1.0, 2.5,
		1.2, 2.5, 1.2
	};


	public:
		MMEval(){
			won = new char[9];
		}

		float H(char *board, uint64_t *mBoardFull, char pl, TicTacEval *ticTacEval){
			float h = 0;

			for(int i = 0; i < 9; i++){
				int l_won = (mBoardFull[i] & TicTacEval::BM_EVAL);
				won[i] = l_won;
				h += toH(l_won) * H_WON * H_MACRO[i];
			}
			
			uint64_t macroEvalPl = ticTacEval->evalPl(won, 2);
			uint64_t macroEvalOp = ticTacEval->evalPl(won, 1);

			for(int i = 0; i < 9; i++){
				if(won[i] != ND){
					continue;
				}	

				int pl = MakroEval(mBoardFull, macroEvalPl, TicTacEval::plb, TicTacEval::plw, TicTacEval::opcw, i); 
				int op = MakroEval(mBoardFull, macroEvalOp, TicTacEval::opb, TicTacEval::opw, TicTacEval::plcw, i); 

				h += (pl * pl - op * op) * H_MACRO[i];
			}
			return h * (pl == X ? 1 : -1);
		}

		int MakroEval(uint64_t *mBoardFull, uint64_t macroEval, int baseP, int baseW, int baseOCw, int i){
			int eval = ((macroEval >> (baseP + i * TicTacEval::shft)) & TicTacEval::BM_EVAL);
			
			// if(eval == 3){
			// 	for(int j = 0; j < 9; j++){
			// 		if((((mBoardFull[i] >> (baseW + j)) & 1) && won[j] == ND)){
			// 			if(((mBoardFull[j] >> (baseOCw) & 1))){
			// 				eval++;
			// 				break;
			// 			}
			// 		}
			// 	}
			// }

			int mEval = ((mBoardFull[i] >> (baseP + 18)) & TicTacEval::BM_EVAL);
			return eval * mEval;
		}

		int toH(int p){
			switch(p){
				case 1:
					return 1;
				case 2:
					return -1;
				default:
					return 0;
			}
		}
};
