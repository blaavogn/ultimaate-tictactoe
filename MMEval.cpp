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
	char *missingSumPl; 
	char *missingSumOp; 
		
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	float H_POS_MACRO[9] = {
		2.0, 1.0, 2.0,
		1.0, 4.0, 1.0,
		2.0, 1.0, 2.0};

	float H_EMP = .3;
	float H_WON = 100;
	float h = 0;
	public:
		MMEval(){
			won = new char[9];
			missingSumPl = new char[9];
			missingSumOp = new char[9];
		}

		//CH
		float H(char *board, uint64_t *mBoardFull, char pl, TicTacEval *ticTacEval){
		  h = 0;
			for(int i = 0; i < 9; i++){
				uint64_t b = mBoardFull[i];
				won[i] = b & ticTacEval->BM_EVAL;
				missingSumPl[i] = 0;
				missingSumOp[i] = 0;
				if(won[i] == ND){
					int plV = (b >> (ticTacEval->plb)) & ticTacEval->BM_EVAL_LARGE;
					int opV = (b >> (ticTacEval->opb)) & ticTacEval->BM_EVAL_LARGE;
					h += (plV - opV) * H_EMP * H_POS_MACRO[i];
					missingSumPl[i] = plV;
					missingSumOp[i] = opV;
				}
			};
			uint64_t macro = ticTacEval->eval(won);

			int macroPlV = (macro >> (ticTacEval->plb)) & ticTacEval->BM_EVAL_LARGE;
			int macroOpV = (macro >> (ticTacEval->opb)) & ticTacEval->BM_EVAL_LARGE;
			h += (macroPlV - macroOpV) * 200;

			for(int i = 0; i < 9; i++){								
				// if(won[i] == 1){
					addDir(1, (i / 3) * 3, 1, 1, missingSumPl, missingSumOp); //Horizontal
					addDir(1, i % 3, 3, 1, missingSumPl, missingSumOp); //Vertical
					
					if(i == 0 || i == 4 || i == 8)
						addDir(1, 0, 4, 1, missingSumPl, missingSumOp); //Vertical
					
					if(i == 2 || i == 4 || i == 6)
						addDir(1, 2, 2, 1, missingSumPl, missingSumOp); //Vertical
				// }
				// if(won[i] == 2){
					addDir(2, (i / 3) * 3, 1, -1, missingSumOp, missingSumPl); //Horizontal
					addDir(2, i % 3, 3, -1, missingSumOp, missingSumPl); //Vertical
					
					if(i == 0 || i == 4 || i == 8)
						addDir(2, 0, 4, -1, missingSumOp, missingSumPl); //Vertical
					
					if(i == 2 || i == 4 || i == 6)
						addDir(2, 2, 2, -1, missingSumOp, missingSumPl); //Vertical
				// }
			}
			
			return h * (pl == X ? 1 : -1);
		}

		void addDir(int pl, int base, int step, int hDir, char* plSum, char* opSum){
			int c = 0;
			int w = 0;
			float lH = 1;
			for(int i = 0; i < 3; i++){
				int index = base + i * step;
				if(won[index] != pl && won[index] != 0){
					return;
				}
				if(won[index] == 0){
					c++;
				}else{
					w = 1;
				}
			}

			for(int i = 0; i < 3; i++){
				int index = base + i * step;
				if(won[index] == pl){
					lH *= 1 * hDir * H_POS_MACRO[i];
				}else{
					if(c == 2 || w == 0){
						lH *= plSum[index] * hDir * .5 - opSum[index] * hDir * .5;
					}else{
						lH *= plSum[index] * hDir * 100  - opSum[index] * hDir * .5;
					}
				}
			}
			h += lH;
		}
};
