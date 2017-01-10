#include <iostream>
#include "TicTacEval.cpp"

class MMEval{
	char *won; 
	char *plH; 
	char *opH; 

	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;
	
	float H_MACRO[9] = {
		1.2, 1.0, 1.2,
		1.0, 3.0, 1.0,
		1.2, 1.0, 1.2
	};

	public:
		MMEval(){
			won = new char[9];
			plH = new char[9];
			opH = new char[9];
		}

		float H(char *board, uint64_t *mBoardFull, char pl, TicTacEval *ticTacEval){
			float h = 0;

			for(int i = 0; i < 9; i++){
				int l_won = mBoardFull[i] & TicTacEval::BM_EVAL;
				won[i] = l_won;
				h += toH(l_won) * 1000;				
				plH[i] = 0;
				opH[i] = 0;
				if(l_won == 1)
					plH[i] = 2;
				if(l_won == 2)
					opH[i] = 2;
			}
			
			uint64_t macroEvalPl = ticTacEval->evalPl(won, 2);
			uint64_t macroEvalOp = ticTacEval->evalPl(won, 1);

			for(int i = 0; i < 9; i++){
				if(won[i] != ND){
					continue;
				}	

				plH[i] = MakroEval(mBoardFull, macroEvalPl, TicTacEval::plb, TicTacEval::plw, TicTacEval::opcw, i); 
				opH[i] = MakroEval(mBoardFull, macroEvalOp, TicTacEval::opb, TicTacEval::opw, TicTacEval::plcw, i); 

				h += (plH[i] * plH[i] - opH[i] * opH[i]) * 2000 * H_MACRO[i];
			}
			return (pl == X ? 1 : -1) * (h +
				plH[0] * plH[1] * plH[2] +
				plH[3] * plH[4] * plH[5] +
				plH[6] * plH[7] * plH[8] +
				plH[0] * plH[3] * plH[6] +
				plH[1] * plH[4] * plH[7] +
				plH[2] * plH[5] * plH[8] +
				plH[0] * plH[4] * plH[8] +
				plH[2] * plH[4] * plH[6] -
				(
					opH[0] * opH[1] * opH[2] +
					opH[3] * opH[4] * opH[5] +
					opH[6] * opH[7] * opH[8] +
					opH[0] * opH[3] * opH[6] +
					opH[1] * opH[4] * opH[7] +
					opH[2] * opH[5] * opH[8] +
					opH[0] * opH[4] * opH[8] +
					opH[2] * opH[4] * opH[6] 
				)
			);
		}

		int MakroEval(uint64_t *mBoardFull, uint64_t macroEval, int baseP, int baseW, int baseOCw, int i){
			int eval = ((macroEval >> (baseP + i * TicTacEval::shft)) & TicTacEval::BM_EVAL);
			
			if(eval == 3){
				for(int j = 0; j < 9; j++){
					int c = 0;
					if(((mBoardFull[i] >> (baseW + j)) & 1) && won[j] == ND){
						if(!((mBoardFull[j] >> (baseOCw) & 1) && j != 4) || c == 1){
							eval++;
							break;
						} else{
							// c++;
						}
					}
				}
			}

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
