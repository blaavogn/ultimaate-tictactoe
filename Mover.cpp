#include "TicTacEval.cpp"

class Mover{
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	char *board;
	char *mBoard;
	uint64_t *mBoardFull;
	char *plCanWinMap;
	char *opCanWinMap;

	int *randMoves;
	int *moveHeur;

	char* wildcardMoves;
	char* loosingMoves;

	public:
		Mover(char *inBoard, uint64_t *inMBoardFull, char *inMBoard, char *inPlCanWinMap, char *inOpCanWinMap, int *inRandMoves){
			board = inBoard;
			mBoard = inMBoard;
			mBoardFull = inMBoardFull;
			randMoves = inRandMoves;
			plCanWinMap = inPlCanWinMap;
			opCanWinMap = inOpCanWinMap;
			moveHeur = new int[81];

			wildcardMoves = new char[81];
			loosingMoves = new char[81];
		}

		void getMoves(int* moves, int prevMove, int player, int *qMoves, int prefMove, int *wcMoves, int turn){
			int macroIndex = prevMove % 9;
			int minLim = 0;
			int maxLim = 81;

			if(prevMove > -1 && mBoard[macroIndex] == ND){ //Strict macro placement
				minLim = macroIndex * 9;
				maxLim = minLim + 9;
			}

			int normalCount = 0; 
			int wildcardCount = 0;
			int loosingCount = 0;
			int opCanWinWithWild = 0;

			int opCanWinMacro = (player == X) ? TicTacEval::opcw : TicTacEval::plcw;
			char* opwinMap = (player == X) ? opCanWinMap : plCanWinMap;
	
			for(int i = 0; i < 9; i++){
				if(opwinMap[i]){
					opCanWinWithWild = 1;
				}
			}

			if(prefMove != -1){
				moves[normalCount++] = prefMove;				
			}
			
			for(int i = minLim; i < maxLim; i++){	
				if(mBoard[i / 9] != ND){
					i += 8;
					continue; //Skipping macros that is done
				}
			
				int mv = randMoves[i];
				if(mv == prefMove || board[mv] != 0){
					continue;	
				}

				int iMod = mv % 9;

				if(opwinMap[iMod] && ((mBoardFull[iMod] >> opCanWinMacro) & TicTacEval::BM_EVAL) == 1){
					loosingMoves[loosingCount++] = mv;
					continue;
				}

				if(mBoard[iMod] != ND){
					if(opCanWinWithWild){
						loosingMoves[loosingCount++] = mv;
						continue;		
					}else{
						wildcardMoves[wildcardCount++] = mv;
						continue;
					}
				}

				moveHeur[normalCount] = ((mBoardFull[i] >> TicTacEval::plb * i % 9) + (mBoardFull[i] >> TicTacEval::opb * i % 9)) * turn;
				moves[normalCount] = mv;
				
				normalCount++;
			}

			*wcMoves = normalCount;
			
			while(wildcardCount>0)
				moves[normalCount++] = wildcardMoves[--wildcardCount];

			*qMoves = normalCount;

			while(loosingCount>0)
				moves[normalCount++] = loosingMoves[--loosingCount];

			moves[normalCount] = 999;					
		}
};