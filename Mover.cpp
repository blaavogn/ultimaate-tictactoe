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
	

	public:
		Mover(char *inBoard, uint64_t *inMBoardFull, char *inMBoard, int *inRandMoves){
			board = inBoard;
			mBoard = inMBoard;
			mBoardFull = inMBoardFull;
			randMoves = inRandMoves;
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
			int playerWinMacro = (player == X) ? TicTacEval::plw : TicTacEval::opw;
			int prefMoveIsNotQMove = 1;

			for(int i = minLim; i < maxLim; i++){	
				if(mBoard[i / 9] != ND){
					i += 8;
					continue; //Skipping macros that is done
				}
				int mv = randMoves[i];
				if(mv == prefMove){
					prefMoveIsNotQMove = (((mBoardFull[mv / 9] >> (playerWinMacro + mv % 9)) & 1) == 1) ? 0 : 1;
					continue;	
				}

				if(board[mv] == 0){
					int iMod = mv % 9;

					if(((mBoardFull[mv / 9] >> (playerWinMacro + iMod)) & 1) == 1){
						qCount++; //Qsearch move
					
						if(lCount > bCount){
							int tmp = moves[bCount];
							moves[bCount++] = mv;
							moves[lCount++] = tmp;
						}else{
							bCount++;
							moves[lCount++] = mv;
						}
					}else	if(((mBoardFull[iMod] >> opCanWinMacro) & TicTacEval::BM_EVAL) != 1 && mBoard[iMod] == ND){
						//Normal move
						moves[lCount++] = mv;
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

			*qMovesLow = prefMoveIsNotQMove;
			*qMoves = qCount;
			moves[lCount] = 999;					
		}

};