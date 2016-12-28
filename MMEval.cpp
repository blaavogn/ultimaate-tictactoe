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

	float H_WON = 10;
	float H_WILLWIN = 100;

	public:
		MMEval(){
			won = new char[9];
			willWinPl = new char[9];
			willWinOp = new char[9];
			missingOnePl = new char[9];
			missingOneOp = new char[9];
		}

		//CH1
		// float H(char *board, uint64_t *mBoardFull, char pl, TicTacEval *ticTacEval){
		// 	float h = 0;
		// 	for(int i = 0; i < 18; i++){
		// 		float v = 0;
		// 		if(board[i] == 1)
		// 			v = 1;
		// 		if(board[i] == 2)
		// 			v = -1;
		// 		h += H_WON * v;
		// 	}
		// 	return h * (pl == X ? 1 : -1);
		// }
		//CH1
		float H(char *board, uint64_t *mBoardFull, char pl, TicTacEval *ticTacEval){
			float h = 0;
			for(int i = 0; i < 9; i++){
				auto w = mBoardFull[i] & ticTacEval->BM_EVAL;
				won[i] = w;
				h += H_WON * (w == 1 ? 1 : (w == 2) ? -1 : 0);
			}
			uint64_t fullEval = ticTacEval->eval(won);
			for(int i = 0; i < 9; i++){

				// missingOnePl[i] = (((w >> ( 2 + i * 2)) & ticTacEval->BM_EVAL) > 2) ?  1 : 0;
				// missingOneOp[i] = (((w >> (22 + i * 2)) & ticTacEval->BM_EVAL) > 2) ? -1 : 0;
			
				willWinPl[i]    = ((fullEval >> ( 2 + i * 2)) & ticTacEval->BM_EVAL) > 2 ?  1 : 0;
				willWinOp[i]    = ((fullEval >> (22 + i * 2)) & ticTacEval->BM_EVAL) > 2 ? -1 : 0;
				h += H_WON * willWinPl[i];
				h += H_WON * willWinOp[i];	
			}
			return h * (pl == X ? 1 : -1);
		}
		
		//CH1
		// float H(char *board, uint64_t *mBoardFull, char pl, TicTacEval *ticTacEval){
		// 	float h = 0;
		// 	for(int i = 0; i < 9; i++){
		// 		auto w = mBoardFull[i] & ticTacEval->BM_EVAL;
		// 		won[i] = w;
		// 		h += H_WON * (w == 1 ? 1 : (w == 2) ? -1 : 0);
		// 	}
		// 	uint64_t fullEval = ticTacEval->eval(won);
		// 	for(int i = 0; i < 9; i++){

		// 		// missingOnePl[i] = (((w >> ( 2 + i * 2)) & ticTacEval->BM_EVAL) > 2) ?  1 : 0;
		// 		// missingOneOp[i] = (((w >> (22 + i * 2)) & ticTacEval->BM_EVAL) > 2) ? -1 : 0;
			
		// 		willWinPl[i]    = ((fullEval >> ( 2 + i * 2)) & ticTacEval->BM_EVAL) > 2 ?  1 : 0;
		// 		willWinOp[i]    = ((fullEval >> (22 + i * 2)) & ticTacEval->BM_EVAL) > 2 ? -1 : 0;
		// 		h += H_WON * willWinPl[i];
		// 		h += H_WON * willWinOp[i];	
		// 	}
		// 	return h;
		// }
	};

		// float H(char *board, char *lmBoard, char pl, TicTacEval *ticTacEval){
		// 	float h = 0;
		// 	uint64_t eval = ticTacEval->eval(lmBoard);

		// 	for(int i = 0; i < 9; i++){
		// 		tmpBoard[i] = 0;
		// 	}
			
		// 	int free = 0;
		// 	for(int i = 0; i < 9; i++){
		// 		if(lmBoard[i] != ND){
		// 			continue;
		// 		}
		// 		int in = i * 9;
		// 		for(int j = 0; j < 9; j++){
		// 			int v = board[in + j];
		// 			if(v != 0){
		// 				free += (v == X) ? 12 : -12;
		// 			}else{
		// 				tmpBoard[j] += 1;
		// 			}
		// 		}
		// 	}

		// 	h += free;

		// 	int plBest = 0;
		// 	int opBest = 0;

		// 	for(int i = 0; i < 9; i++){
		// 		int lm = lmBoard[i];
		// 		if(lm == DR){
		// 			continue;
		// 		}

		// 		int t = (lmBoard[i] == ND ? 0 : (lmBoard[i] == X ? 1 : -1)) * 15;
		// 		h += t;
				
		// 		int sh = i * 2;
		// 		int plCon = (eval >> (sh + 2)) & ticTacEval->BM_EVAL; 
		// 		int opCon = ((eval >> (sh + 22)) & ticTacEval->BM_EVAL); 

		// 		plBest = std::max(plCon - 2, plBest);
		// 		opBest = std::max(opCon - 2, opBest);
		// 		t = plCon * 3 * tmpBoard[i] + opCon * -3 * tmpBoard[i];

		// 		h += t;
		// 	}

		// 	h += plBest * 100 - opBest * 100;
		// 	return h * (pl == X ? 1 : -1);
		// };
