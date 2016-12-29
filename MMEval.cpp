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
		1.0, 2.0, 1.0,
		1.0, 1.0, 1.0};

	float H_EMP = 0.07;
	float H_MISSING_ONE = .3;
	float H_WON = 70;
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
				missingOneSumPl[i] = 0;
				missingOneSumOp[i] = 0;

				uint64_t b = mBoardFull[i];
				float w = b & ticTacEval->BM_EVAL;
				won[i] = w;
				h += H_POS_MACRO[i] * H_WON * (w == 1 ? 1 : (w == 2) ? -1 : 0);

				missingOnePl[i] = ((b >> (20)) & ticTacEval->BM_EVAL);
				missingOneOp[i] = ((b >> (40)) & ticTacEval->BM_EVAL);

				if(w == ND){
					for(int j = 0; j < 9; j++){
						int plV = (b >> (2 + 2 * j)) & ticTacEval->BM_EVAL;
						int opV = (b >> (22 + 2 * j)) & ticTacEval->BM_EVAL;
						missingOneSumPl[i] += plV;
						missingOneSumOp[i] += opV;
						h += (plV - opV) * H_EMP;
					}
				}
			}
			uint64_t fullEval = ticTacEval->eval(won);
			for(int i = 0; i < 9; i++){								
				float plWin = ((fullEval >> ( 2 + i * 2)) & ticTacEval->BM_EVAL) > 2 ?  1 : 0;
				float opWin = ((fullEval >> (22 + i * 2)) & ticTacEval->BM_EVAL) > 2 ? -1 : 0;
				willWinPl[i] = plWin;
				willWinOp[i] = opWin;
				h += H_WILLWIN * plWin; //OVERSER HVIS DEN MELLEMLIGGENDE ER MEGET SVÆR AT TAGE
				h += H_WILLWIN * opWin;
				for(int j = 0; j < 9; j++){
					if(board[i * 9 + j] == 1){
						h += H_WILLWIN_BONUS * (plWin);
					}
					if(board[i * 9 + j] == 2){
						h += H_WILLWIN_BONUS * (opWin);
					}
					if(board[i + j * 9] == 0){
						h += H_WILLWIN_BONUS * (plWin + opWin);
					}
				}	
			}

			for(int i = 0; i < 9; i++){			
				if(missingOnePl[i] == 3){
					int cl = 0;
					for(int j = 0; j < 9; j++){
						if(i == j){
							continue;
						}
						if(willWinOp[j] != 0 && won[j] == 0){
							cl = 1;
						}
					}
		
					h += H_MISSING_ONE * cl;
					h += H_MISSING_ONE_WILL_WIN * willWinPl[i];
				}
			
				if(missingOneOp[i] == 3){
					int cl = 0;
					for(int j = 0; j < 9; j++){
						if(j == i){
							continue;
						}
						if(willWinPl[j] != 0 && won[j] == 0){
							cl = 1;
						}
					}
					
					h += H_MISSING_ONE * cl * -1;
					h += H_MISSING_ONE_WILL_WIN * willWinOp[i];
				}
				h += (H_MISSING_ONE * missingOnePl[i] * missingOneSumPl[i]) + 
				     (H_MISSING_ONE * missingOneOp[i] * missingOneSumOp[i] * -1);
			}
			
			return h * (pl == X ? 1 : -1);
		}
};
