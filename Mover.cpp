#include "TicTacEval.cpp"

class Mover{
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	char *board;
	char *mBoard;
	uint64_t *mBoardFull;
	int *randMoves;
	int *moveHeur;	

	public:
		Mover(char *inBoard, uint64_t *inMBoardFull, char *inMBoard, int *inRandMoves){
			board = inBoard;
			mBoard = inMBoard;
			mBoardFull = inMBoardFull;
			randMoves = inRandMoves;
			moveHeur = new int[81];
		}

		void getMoves(int* moves, int prevMove, int player, int *qMoves, int *qMovesLow, int prefMove){
			int macroIndex = prevMove % 9;
			int minLim = 0;
			int maxLim = 81;

			if(prevMove > -1 && mBoard[macroIndex] == ND){ //Strict macro placement
				minLim = macroIndex * 9;
				maxLim = minLim + 9;
			}

			int bCount = 0, lCount = 0, hCount = 81, qCount = 0;
			if(prefMove != -1){
				moves[0] = prefMove;
				lCount++;
				bCount++;
			}

			int opCanWinMacro   = (player == X) ? TicTacEval::opcw : TicTacEval::plcw;
			int opMicroHeur     = (player == X) ? TicTacEval::opb : TicTacEval::plb;
			// int playerWinMacro = (player == X) ? TicTacEval::plw : TicTacEval::opw;
			
			for(int i = minLim; i < maxLim; i++){	
				if(mBoard[i / 9] != ND){
					i += 8;
					continue; //Skipping macros that is done
				}
				int mv = randMoves[i];
				if(mv == prefMove){
					continue;	
				}

				if(board[mv] == 0){
					int iMod = mv % 9;
					int iFlor = mv / 9;

					if(((mBoardFull[iMod] >> opCanWinMacro) & TicTacEval::BM_EVAL) != 1 && mBoard[iMod] == ND){
						//Normal move
						moveHeur[lCount] = (mBoardFull[iFlor] >> (opMicroHeur + TicTacEval::shft * iMod)) & TicTacEval::BM_EVAL; 
						moves[lCount++] = mv;
						for(int j = lCount - 1; j > 0; j--){
							if(moveHeur[j] > moveHeur[j - 1]){
								int tmp1 = moveHeur[j];
								int tmp2 = moves[j];
								moveHeur[j] = moveHeur[j - 1];
								moves[j] = moves[j - 1];
								moveHeur[j - 1] = tmp1;
								moves[j - 1] = tmp2;
							}
						}

					}else{
						//Bad move
						moves[hCount++] = mv;
					}
				}
			}

			while(hCount > 81){
				int i = moves[--hCount];
				moves[lCount++] = i;
			}

			*qMovesLow = 0;
			*qMoves = qCount;
			moves[lCount] = 999;					
		}

};