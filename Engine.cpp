#include <iostream>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "HashBoard.cpp"
#include "TranspositionTable.cpp"
#include "TicTacEval.cpp"
#include "MMEval.cpp"
#include "Printer.cpp"

class Engine{
	float mynan = 0.0/0.0 * -1;
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	char *board;
	char *mBoard;
	char *mPlaceWon;
	uint64_t *mBoardFull;
	int *movePool;
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
	int hashDepth = 14;
	int ponder = 0;
	MMEval* evaluater;
	float gl_Eval = 0.0;
	TranspositionTable* transTable;
	TicTacEval* ticTacEval;
	HashBoard* hashKey;
	Printer* printer;

	public:
		Engine(){
			board     	= new char[81];
			mBoard 			= new char[9];
			mPlaceWon 	= new char[9];
			mBoardFull 	= new uint64_t[9];
			boardHash   = new std::size_t[9];
			movePool 	  = new int[84 * 81];
			transTable  = new TranspositionTable();
			hashKey 		= new HashBoard();
			ticTacEval  = new TicTacEval();
			evaluater   = new MMEval();
			printer     = new Printer();
			hashKey->board = board;
			gl_PrevMove = -1;
			ponder  	  = 1;
			movesMade   = 0;

			for(int i = 0; i < 9; i++){
				mBoard[i] = 0;
				mBoardFull[i] = 0;
				boardHash[i] = 0;
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
			for(depth = 3; depth < 20; depth++){
				itDepth = depth;

				gl_Eval = Negamax(gl_PrevMove, depth, -INFINITY, INFINITY, 1);
				// gl_Eval = MTDF(gl_Eval, gl_PrevMove, depth);
				
				gettimeofday(&tp, NULL);
				long int ms2 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
				sec = ((float) (ms2-ms) / 1000.0);
				// fprintf(stderr, "%d - %f - %f\n", depth, gl_Eval, sec);
				if(time < 5000 && sec > 0.200)
					break;
				if(time < 2500 && sec > 0.110)
					break;
				if(sec > 0.350)
					break;
			}
	
			hashKey->prevMove = gl_PrevMove % 9;
			hashKey->hash = hash;
			auto transPos = transTable->find(hashKey);
			int chosenMove = -1;

			if(transPos != transTable->end()){
				chosenMove = transPos->second;
			}else{
				fprintf(stderr, "Error, chosen move not found in transposition table, we are fucked\n");
			}

			fprintf( stderr, "%d: Depth %d: %2.3f mN/sec - %2.3f sec\n", movesMade, depth, ((float) mNodes / 1000000.0) / sec, sec);
			fprintf( stderr, "TransHit: %d Full: %d, transsize: %ld \n", transHit, transHitFull, transTable->size());
			fprintf( stderr, "Nodes: %d LMR_Re: %d\n", mNodes, LMR_Re);
			fprintf( stderr, "Best move: %d, %f\n", chosenMove, gl_Eval);

			// fprintf(stderr, "PV: \n");
			// HashBoard* pvKey = new HashBoard();
			// pvKey->board = new char[81];
			// pvKey->hash = hash;
			// for(int i = 0; i < 81; i++){
			// 	pvKey->board[i] = board[i];				
			// }

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
			// //delete(pvKey);
			// fprintf(stderr, "\n");
			
			if(chosenMove == -1){
				int k = 0;
				getMoves(movePool, gl_PrevMove, pl, &k, -1);
				chosenMove = movePool[0];
			}

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
		float MTDF(float f, int prevMove, int depth){
      float g = 0.0;
      float upperBound = INFINITY;
      float lowerBound = -INFINITY;
      while (lowerBound < upperBound){
    		float beta = (g > lowerBound+1) ? g : lowerBound+1;
    		g = Negamax(prevMove, depth, beta-1, beta, 1);
  		 	if (g < beta){
          upperBound = g;
  		 	}
       	else{
          lowerBound = g;
       	}
      }
     return g;
    }

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
				if(transPos->first->itDepth == itDepth + depth && !transPos->first->cut && !isnan(transPos->first->eval)){
					transHitFull++;
					return transPos->first->eval;
				}
			}

			//Teminate position?
			char v = ValidateMacroDelta(prevMove / 9);

			int qMoves = 0;
			int* moves = &movePool[82 * (80 - (depth <= 0 ? 0 : depth))];
			
			getMoves(moves, prevMove, token, &qMoves, prefMove);

			if(v != ND && v != DR){
				return -INFINITY;
			}
			else if(depth <= 0){
				if(qMoves > 0){
					return qSearch(prevMove, turn, 78);
				}
				return evaluater->H(board, mBoardFull, pl, ticTacEval) * turn;
			}
			
			float extreme = -INFINITY;
			float extremeT = -INFINITY;
			char cut = 0;
			int chosenMove = -1;
			for(int c = 0, i = -1;; ){
				i = moves[c++];
				if( i == 999){
					break;
				}
				int macroIndex = i / 9;

				board[i] = token;
				uint64_t oldMBoardValue = mBoardFull[macroIndex];
				boardHash[macroIndex] += hash_tts[token][i % 9];
				uint64_t newMBoardValue = ticTacEval->evalPure(boardHash[macroIndex]);
				mBoard[macroIndex] = newMBoardValue & ticTacEval->BM_EVAL;
				mBoardFull[macroIndex] = newMBoardValue;
				std::size_t hashChange = (token == X) ? hash_pl[i] : hash_op[i];
				hash = hash ^ hashChange;

				float e;
				if(c < 3){
					e = Negamax(i, depth - 1, beta * -1, alpha * -1, turn * -1) * -1;
				}else{
					e = Negamax(i, depth - 2, beta * -1, alpha * -1, turn * -1) * -1;
					if(e > alpha){
						LMR_Re++;
						e = Negamax(i, depth - 1, beta * -1, alpha * -1, turn * -1) * -1;
					}
				}

				hash = hash ^ hashChange;

				mBoardFull[macroIndex] = oldMBoardValue;
				mBoard[macroIndex] = oldMBoardValue & ticTacEval->BM_EVAL;
				boardHash[macroIndex] -= hash_tts[token][i % 9];

				board[i] = 0;
				if(isnan(e)){
					continue;
				}
				if(e >= extreme){
					extreme = e;
					extremeT = e;
					chosenMove = i;
				}
				alpha = fmax(alpha, e);

				if(alpha >= beta){
					extremeT = alpha;
					extreme = mynan;
					cut = 1;
					break;
				}
			}

			if(itDepth - depth < hashDepth){
				if(transPos != transTable->end()){
					TranspositionTable::UpdateTransPos(transPos, chosenMove, extremeT, itDepth + depth, cut);
				}else{
					transTable->insert(chosenMove, board, hash, extremeT, prevMove % 9, itDepth + depth, movesMade + (itDepth - depth), cut);
				}
			}
			return extreme;
		} 

		float qSearch(int prevMove, int turn, int depth){
			mNodes++;

			int token = (turn == 1) ? pl : op;
				
			//Teminate position?
			char v = ValidateMacroDelta(prevMove / 9);
			if(v != ND && v != DR){
				return -INFINITY;
			}

			int qMoves;
			int* moves = &movePool[82 * (80 - depth)];
			getMoves(moves, prevMove, token, &qMoves, -1);

			if(qMoves == 0){
				return evaluater->H(board, mBoardFull, pl, ticTacEval) * turn * -1;
			}

			float extreme = -INFINITY;
			for(int i = 0, c = 0; c < qMoves && i != 999; ){
				i = moves[c++];
				int macroIndex = i / 9;
				board[i] = token;
				uint64_t oldMBoardValue = mBoardFull[macroIndex];
				boardHash[macroIndex] += hash_tts[token][i % 9];
				uint64_t newMBoardValue = ticTacEval->evalPure(boardHash[macroIndex]);
				mBoard[macroIndex] = newMBoardValue & ticTacEval->BM_EVAL;
				mBoardFull[macroIndex] = newMBoardValue;
				float	e = qSearch(i, turn * -1, depth - 1) * -1;

				mBoardFull[macroIndex] = oldMBoardValue;
				mBoard[macroIndex] = oldMBoardValue & ticTacEval->BM_EVAL;
				boardHash[macroIndex] -= hash_tts[token][i % 9];

				board[i] = 0;				
				extreme = fmax(extreme, e);
			}
			return extreme;
		}
		
		void getMoves(int* moves, int prevMove, int player, int *qMoves, int prefMove){
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

			for(int i = minLim; i < maxLim; i++){	
				if(mBoard[i / 9] != ND){
					i += 8;
					continue; //Skipping macros that is done
				} 
				int sh = (player == X) ? 40 : 20;
				int sh_i = (player == X) ? 2 : 22;

				if(board[i] == 0){
					int iMod = i % 9;

					if(((mBoardFull[i / 9] >> (sh_i + 2 * iMod)) & ticTacEval->BM_EVAL) == 3){
						qCount++; //Qsearch move
					
						if(lCount > bCount){
							int tmp = moves[bCount];
							moves[bCount++] = i;
							moves[lCount++] = tmp;
						}else{
							bCount++;
							moves[lCount++] = i;
						}
					}else	if(mBoard[iMod] == ND && ((mBoardFull[iMod] >> sh) & ticTacEval->BM_EVAL) != 3){
						//Normal move
						moves[lCount++] = i;
					}else{
						//Bad move
						moves[hCount++] = i;
					}
				}
			}
			
			while(hCount > 81){
				int i = moves[--hCount];
				moves[lCount++] = i;
			}

			*qMoves = qCount;
			moves[lCount] = 999;					
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