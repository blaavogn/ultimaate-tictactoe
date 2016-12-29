#include <iostream>
#include "TicTacEval.cpp"

class MMEval{
// 	\|012|345|678
//  -+---+---+---
//  0|X  |OXX|O··
//  1|   |X·X|X··
//  2|OOO|··O|··O
//  -+---+---+---
//  3|O O|·O·|O··
//  4| OO|OX·|X·O
//  5|XOO|O·O|OX·
//  -+---+---+---
//  6|X  |X··|X X
//  7|XXX|XX·|XXX
//  8|  O|O··|   


	char *won; 
	char *willWinPl; 
	char *willWinOp; 
	char *missingOnePl; 
	char *missingOneOp; 
		
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	float H_EMP = 0.1;
	float H_MISSING_ONE = 1;
	float H_WON = 10;
	float H_WILLWIN = 100;
	float H_MISSING_ONE_WILL_WIN = 1000;

	public:
		MMEval(){
			won = new char[9];
			willWinPl = new char[9];
			willWinOp = new char[9];
			missingOnePl = new char[9];
			missingOneOp = new char[9];
		}

		//CH
		float H(char *board, uint64_t *mBoardFull, char pl, TicTacEval *ticTacEval){
			float h = 0;
			for(int i = 0; i < 9; i++){
				float w = mBoardFull[i] & ticTacEval->BM_EVAL;
				won[i] = w;
				h += H_WON * (w == 1 ? 1 : (w == 2) ? -1 : 0);
				if(w == ND){
					for(int j = 0; j < 9; j++){
						int v = (board[j] == 1 ? 1 : (board[j] == 2) ? -1 : 0);
						h += v * H_EMP;
					}
				}
			}
			uint64_t fullEval = ticTacEval->eval(won);

			for(int i = 0; i < 9; i++){			
				willWinPl[i]    = ((fullEval >> ( 2 + i * 2)) & ticTacEval->BM_EVAL) > 2 ?  1 : 0;
				willWinOp[i]    = ((fullEval >> (22 + i * 2)) & ticTacEval->BM_EVAL) > 2 ? -1 : 0;

				missingOnePl[i] = ((mBoardFull[i] >> (20 + i * 2)) & ticTacEval->BM_EVAL) > 2 ?  1 : 0;
				missingOneOp[i] = ((mBoardFull[i] >> (40 + i * 2)) & ticTacEval->BM_EVAL) > 2 ? -1 : 0;
				h += H_WILLWIN * willWinPl[i];
				h += H_WILLWIN * willWinOp[i];	

				h += H_MISSING_ONE * missingOnePl[i];
				h += H_MISSING_ONE * missingOneOp[i];	
				
				h += H_MISSING_ONE_WILL_WIN * missingOnePl[i] * willWinPl[i];
				h += H_MISSING_ONE_WILL_WIN * missingOneOp[i] * willWinOp[i];
			}
			return h * (pl == X ? 1 : -1);
		}
};
