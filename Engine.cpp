#include <iostream>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "HashBoard.cpp"
#include "TranspositionTable.cpp"
#include "TicTacEval.cpp"
#include "MMEval.cpp"
#include "Printer.cpp"
#include "Mover.cpp"

class Engine{
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;
	const float CONST_WON = -1000000;

	char *board;
	char *mBoard;
	char *mPlaceWon;
	uint64_t *mBoardFull;
	int *movePool;
	int *movePoolQ;
	std::size_t* boardHash; 
	int gl_PrevMove;
	char pl, op;
	int lMove, mNodes;
	int movesMade;
	std::size_t hash = 0;
	int itDepth = 0;
	int transHitFull = 0;
	int transHit = 0;
	int LMR_Re = 0;
	int hashDepth = 9;
	int ponder = 0;
	MMEval* evaluater;
	TranspositionTable* transTable;
	TicTacEval* ticTacEval;
	Mover* mover;
	HashBoard* hashKey;
	Printer* printer;

	int* randMoves;
  
	public:
		Engine(){
 			board     	= new char[81];
			mBoard 			= new char[9];
			mPlaceWon 	= new char[9];
			mBoardFull 	= new uint64_t[9];
			boardHash   = new std::size_t[9];
			movePool 	  = new int[82 * 81];
			movePoolQ 	= new int[82 * 30];
			transTable  = new TranspositionTable();
			hashKey 		= new HashBoard();
			ticTacEval  = new TicTacEval();
			evaluater   = new MMEval();
			printer     = new Printer();
			randMoves   = new int[81];
			mover 		  = new Mover(board, mBoardFull, mBoard, randMoves);
			hashKey->board = board;
			gl_PrevMove = -1;
			ponder  	  = 1;
			movesMade   = 0;
			
			srand( time( NULL ) );

			for(int i = 0; i < 9; i++){
				mBoard[i] = 0;
				mBoardFull[i] = 0;
				boardHash[i] = 0;
			}

			for(int i = 0; i < 81; i++){
				board[i] = 0;
				randMoves[i] = i;
				if(i % 9 == 7){
					randMoves[i] = i - 4;
					randMoves[i - 4] = i;
				}
			}

			for(int i = 0; i < 9; i++){
				for(int j = 0; j < 7; j++){
					int in = rand() % (7 - j) + j;
					int mv = randMoves[i * 9 + in];
					randMoves[i * 9 + in] = randMoves[i * 9 + j]; 						
					randMoves[i * 9 + j] = mv; 						
				}
			}
		}
	
		void Update(int *inBoard){
			movesMade = 0;
			for(int y = 0; y < 9; y++){
				for(int x = 0; x < 9; x++){
					int index = CTI(x,y);
					int oldValue = board[index];
					int pV = inBoard[x + y * 9];
					int newValue = (pV == 0) ? ND : ((pV == 1) ? X : O);
					
					if(oldValue != newValue){
						movesMade++;
						board[index] = newValue;
						boardHash[index / 9] += hash_tts[newValue][index % 9];
						
						if(pV != pl){
							gl_PrevMove = index;
						}

						if(newValue == X){
							hash = hash ^ hash_op[index];
						}else if(newValue == O){
							hash = hash ^ hash_pl[index]; 
						}
					}
				}
			}	
			printer->Print(board);
		}

		std::pair<int, int> Move(int time){
			if(movesMade == 0){
				return std::make_pair(4,4);
			}

			struct timeval tp;
			gettimeofday(&tp, NULL);
			long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
			float sec = 0.0;
			
			transHitFull = 0;
			transHit = 0;
			mNodes = 0;
			LMR_Re = 0;

			for(int j = 0; j < 9; j++){
				uint64_t v = ticTacEval->eval(board + j * 9);
				mBoard[j] = v & ticTacEval->BM_EVAL;
				mBoardFull[j] = v; 
			}	
				
			int depth;
			for(depth = 4; depth < 25; depth++){
				itDepth = depth;
				auto eval = Negamax(gl_PrevMove, depth, CONST_WON - depth, INFINITY + depth, 1);
				
				gettimeofday(&tp, NULL);
				long int ms2 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
				sec = ((float) (ms2-ms) / 1000.0);
				fprintf(stderr, "%d - %f - %f - %d\n", depth, eval, sec, mNodes);
				if(time < 7500 && sec > 0.350)
					break;
				if(time < 5000 && sec > 0.250)
					break;
				if(time < 2500 && sec > 0.150)
					break;
				if(sec > 0.100)
					break;
			}
	
			hashKey->prevMove = gl_PrevMove % 9;
			hashKey->hash = hash;
			auto transPos = transTable->find(hashKey);
			int chosenMove = -1;
			float move = 0.0;

			if(transPos != transTable->end()){
				chosenMove = transPos->second;
				move = transPos->first->eval;
			}else{
				fprintf(stderr, "Error, chosen move not found in transposition table, we are fucked\n");
			}

			if(chosenMove == -1){
				int k = 0, kk;
				mover->getMoves(movePool, gl_PrevMove, pl, &k, &kk, -1);
				chosenMove = movePool[0];
				fprintf(stderr, "Choosing loose %d\n", chosenMove);
			}

			fprintf( stderr, "%d: Depth %d: %2.3f mN/sec - %2.3f sec\n", movesMade, depth, ((float) mNodes / 1000000.0) / sec, sec);
			fprintf( stderr, "TransHit: %d Full: %d, transsize: %ld \n", transHit, transHitFull, transTable->size());
			fprintf( stderr, "Nodes: %d LMR_Re: %d\n", mNodes, LMR_Re);
			
			if(move < CONST_WON || move > -CONST_WON){
				fprintf( stderr, "Best move: %d, %f # ?? \n", chosenMove, move);
			}else{
				fprintf( stderr, "Best move: %d, %f\n", chosenMove, move);
			}

			fprintf(stderr, "PV: \n");
			HashBoard* pvKey = new HashBoard();
			pvKey->board = new char[81];
			pvKey->hash = hash;
			for(int i = 0; i < 81; i++){
				pvKey->board[i] = board[i];				
			}

			// int tok = pl;

			// while(transPos != transTable->end()){
			// 	int move = transPos->second;
			// 	fprintf(stderr, "(%d,%d),", move / 9, move % 9);
			// 	pvKey->board[(int)move] = tok;
			// 	std::size_t hashChange = ((tok == X) ? hash_pl[move] : hash_op[move]);
			// 	pvKey->hash = pvKey->hash ^ hashChange;
			// 	pvKey->prevMove = move % 9;
			// 	tok = (tok == pl) ? op : pl;
			// 	transPos = transTable->find(pvKey);
			// }
			// fprintf(stderr, "\n");
			// printer->Print(pvKey->board);
			// // delete(pvKey);
			// fprintf(stderr, "\n");

			int macro = chosenMove / 9;
			int micro = chosenMove % 9;
			int x = (macro % 3) * 3 + micro % 3;
			int y = (macro / 3) * 3 + micro / 3;
		
			fprintf( stderr, "%d, %d\n", x,y);
			ponder = 0;
			return std::make_pair(x, y);	
		}

		void Ponder(){
			if(!ponder){
				ponder = 1;
				transTable->cleanUp(movesMade + 1); //Should be done on pondering.
			}
		}

		void SetPlayer(char pl){
			this->pl = pl;
		  op = (pl == 1) ? 2 : 1;  
		}

	private:
		float Negamax(int prevMove, int depth, float alpha, float beta, int turn){
			mNodes++;
			int token = (turn == 1) ? pl : op;
				
			//Check transtable
			hashKey->prevMove = prevMove % 9;
			hashKey->hash = hash;
			auto transPos = transTable->find(hashKey);
			
			char prefMove = -1;
			if(transPos != transTable->end()) {
				prefMove = transPos->second;
				transHit++;
				if(transPos->first->itDepth == itDepth + depth){
					transHitFull++;
					return transPos->first->eval;
				}
			}

			//Teminate position?
			char v = ValidateMacroDelta(prevMove / 9);

			int qMoves = 0, qLow = 0;
			int* moves = &movePool[82 * (80 - (depth <= 0 ? 0 : depth))];
			mover->getMoves(moves, prevMove, token, &qMoves, &qLow, prefMove);
			if(moves[0] == 999){
				return 0.0;
			}
			if(v != ND && v != DR){
				return CONST_WON;
			}else if(depth <= 0){
				// if(qMoves > 0){
				// 	// return qSearch(prevMove, turn, 0, alpha, beta);
				// }
				return evaluater->H(board, mBoardFull, pl, ticTacEval) * turn;
			}
			
			float extreme = -INFINITY;
			int chosenMove = -1;

			char isCut = 0;
		
			for(int c = 0, i = -1;; ){
				i = moves[c++];
				if(i == 999){
					break;
				}

				auto hashChanges = ApplyMove(i, token);

				float e;
				// if(c < 6 && itDepth - depth  > 3){
					e = -Negamax(i, depth - 1, -beta, -alpha, -turn);
				// }else{
				//   e = -Negamax(i, depth - 2, -beta, -alpha, -turn);
				// 	if(e >= alpha){ //HER ER PROBLEMER
				// 		LMR_Re++;
				// 		e = -Negamax(i, depth - 1, -beta, -alpha, -turn);
				// 	}
				// }

				RemoveMove(i, token, hashChanges);

				if(e >= extreme){
					extreme = e;
					chosenMove = i;
				}

				alpha = std::max(e, alpha);

				if(alpha > beta){
					isCut = 1;
					break;
				}
			}

			if(itDepth - depth < hashDepth){
				if(transPos != transTable->end()){
					TranspositionTable::UpdateTransPos(transPos, chosenMove, extreme, itDepth + depth, isCut);
				}else{
					transTable->insert(chosenMove, board, hash, extreme, prevMove % 9, itDepth + depth, movesMade + (itDepth - depth), isCut);
				}
			}
			return extreme;
		} 

		float qSearch(int prevMove, int turn, int depth, float alpha, float beta){
			mNodes++;
			int token = (turn == 1) ? pl : op;	
			
			//Teminate position?
			char v = ValidateMacroDelta(prevMove / 9);
			if(v != ND && v != DR){
				return CONST_WON - depth;
			}

			int qMoves = 0, qLow = 0;
			
			int* moves = movePoolQ + (28 - depth) * 81;
			
			mover->getMoves(moves, prevMove, token, &qMoves, &qLow, -1);

			if(qMoves == 0){
				return evaluater->H(board, mBoardFull, pl, ticTacEval) * turn;
			}

			float extreme = -INFINITY;
			for(int i = 0, c = qLow; c < qMoves + qLow && i != 999; ){
				i = moves[c++];
				if(i == 999){
					return 0.0;
				}

				board[i] = token;	
				float	e = -qSearch(i, -turn, depth + 1, -beta, -alpha);
				board[i] = 0;	

				extreme = fmax(extreme, e);
				alpha = fmax(alpha, e);

				if(alpha >= beta){
					break;
				}
			}
			return extreme;
		}

		std::pair<uint64_t, std::size_t> ApplyMove(int move, int token){
			int macroIndex = move / 9;
			board[move] = token;
			
			uint64_t oldMBoardValue = mBoardFull[macroIndex];
			boardHash[macroIndex] += hash_tts[token][move % 9];
			uint64_t newMBoardValue = ticTacEval->evalPure(boardHash[macroIndex]);
			mBoard[macroIndex] = newMBoardValue & ticTacEval->BM_EVAL;
			mBoardFull[macroIndex] = newMBoardValue;
			std::size_t hashChange = (token == X) ? hash_pl[move] : hash_op[move];
			hash = hash ^ hashChange;
			return std::make_pair(oldMBoardValue, hashChange);	
		}

		void RemoveMove(int move, int token, std::pair<uint64_t, std::size_t> changes){
			int macroIndex = move / 9;
			mBoardFull[macroIndex] = changes.first;
			mBoard[macroIndex] = changes.first & ticTacEval->BM_EVAL;
			boardHash[macroIndex] -= hash_tts[token][move % 9];
			
			hash = hash ^ ((std::size_t)changes.second);
			board[move] = 0;
		}
	
		char ValidateMacroDelta(int move){
			if(checkDir(mBoard, move - move % 3, 1))
				return (mBoard[move] == 1) ? X : O;
			if(checkDir(mBoard, move - (move / 3) * 3, 3))
				return (mBoard[move] == 1) ? X : O;

			if(move == 0 || move == 4 || move == 8){
				if(checkDir(mBoard, 0, 4))
					return (mBoard[move] == 1) ? X : O;
			}
			if(move == 2 || move == 4 || move == 6){
				if(checkDir(mBoard, 2, 2))
					return (mBoard[move] == 1) ? X : O;
			}

			return DR;
		}

		char checkDir(char *lmBoard, int base, int step){
			return lmBoard[base] % 3 != 0 && 
			       lmBoard[base] == lmBoard[base + step] && 
			       lmBoard[base] == lmBoard[base + 2 * step];
		}

	  int CTI(int x, int y){
			int macroX = x / 3; 
			int macroY = (y / 3) * 3;
			int macro = macroX + macroY;

			int microX = x % 3; 
			int microY = y % 3 * 3; 
			int micro = microX + microY;
			return micro + macro * 9; 
		}
};