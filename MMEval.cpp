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
	char *missingOneSumPl; 
	char *missingOneSumOp; 
		
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	float H_POS_MACRO[9] = {
		1.0, 1.0, 1.0,
		1.0, 3.0, 1.0,
		1.0, 1.0, 1.0};

	float H_EMP = 0.1;
	float H_MISSING_ONE = .3;
	float H_WON = 100;
	float H_WILLWIN = 150;
	float H_WILLWIN_BONUS = 5;
	float H_MISSING_ONE_WILL_WIN = 0;

	public:
		MMEval(){
			won = new char[9];
			willWinPl = new char[9];
			willWinOp = new char[9];
			missingOnePl = new char[9];
			missingOneOp = new char[9];
			missingOneSumPl = new char[9];
			missingOneSumOp = new char[9];
		}

		//CH
		float H(char *board, uint64_t *mBoardFull, char pl, TicTacEval *ticTacEval){
			float h = 0;
			for(int i = 0; i < 9; i++){
				uint64_t b = mBoardFull[i];
				won[i] = b & ticTacEval->BM_EVAL;

				if(won[i] == ND){
					for(int j = 0; j < 9; j++){
						int plV = (b >> (ticTacEval->plb + ticTacEval->shft * j)) & ticTacEval->BM_EVAL;
						int opV = (b >> (ticTacEval->opb + ticTacEval->shft * j)) & ticTacEval->BM_EVAL;
						h += (plV - opV) * H_EMP * H_POS_MACRO[i];
					}
				}
			};

			uint64_t fullEval = ticTacEval->eval(won);
			for(int i = 0; i < 9; i++){								
				if(won[i] == 1){
					h += H_WON * ((fullEval >> (ticTacEval->plb + ticTacEval->shft * i)) & ticTacEval->BM_EVAL);
				}
				if(won[i] == 2){
					h += H_WON * ((fullEval >> (ticTacEval->opb + ticTacEval->shft * i)) & ticTacEval->BM_EVAL) * -1;
				}
			}
			
			return h * (pl == X ? 1 : -1);
		}
};
