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
	uint64_t *mBoardFull;

	int *pearsonValues;

	char *plCanWinMap;
	char *opCanWinMap;

	char plCanWin;
	char opCanWin;

	char *plNoneForcingMoves;
	char *opNoneForcingMoves;

	int *movePool;
	int *movePoolQ;
	std::size_t* boardHash; 
	int gl_PrevMove;
	char pl, op;
	int lMove, mNodes, qNodes;
	int movesMade;
	uint64_t hash = 0;
	int itDepth = 0;
	int transHitFull = 0;
	int transHit = 0;
	int LMR_Re = 0;
	int LMR_NONE = 0;
	int hashDepth = 8;
	int ponder = 0;

	MMEval* evaluater;
	TranspositionTable* transTable;
	TicTacEval* ticTacEval;
	Mover* mover;
	HashKey* hashKey;
	Printer* printer;

	int* randMoves;
  
	public:
		Engine(){
 			board     	= new char[81];
			mBoard 			= new char[9];
			mBoardFull 	= new uint64_t[9];
			boardHash   = new std::size_t[9];
			movePool 	  = new int[82 * 81];
			movePoolQ 	= new int[82 * 30];
			pearsonValues = new int[21];

			plCanWinMap = new char[9];
			opCanWinMap = new char[9];
			plCanWin    = 0;
			opCanWin    = 0;
			plNoneForcingMoves = new char[9];
			opNoneForcingMoves = new char[9];

			transTable  = new TranspositionTable();
			hashKey 		= new HashKey();
			hashKey->board = board;
			ticTacEval  = new TicTacEval();
			evaluater   = new MMEval();
			printer     = new Printer();
			randMoves   = new int[81];
			mover 		  = new Mover(board, mBoardFull, mBoard, plCanWinMap, opCanWinMap, randMoves);
			gl_PrevMove = -1;
			ponder  	  = 1;
			movesMade   = 0;
			
			srand( time( NULL ) );

			for(int i = 0; i < 21; i++){
				pearsonValues[i] = 0;
			}

			for(int i = 0; i < 9; i++){
				mBoard[i] = 0;
				mBoardFull[i] = 0;
				boardHash[i] = 0;
				plCanWinMap[i] = 0;
				opCanWinMap[i] = 0;
				plNoneForcingMoves[i] = 9;
				opNoneForcingMoves[i] = 9;
			}

			for(int i = 0; i < 81; i++){
				board[i] = 0;
				randMoves[i] = i;
				if(i % 9 == 8){
					randMoves[i] = i - 4;
					randMoves[i - 4] = i;
				}
			}

			for(int i = 0; i < 9; i++){
				for(int j = 0; j < 8; j++){
					int in = rand() % (8 - j) + j;
					int mv = randMoves[i * 9 + in];
					randMoves[i * 9 + in] = randMoves[i * 9 + j]; 						
					randMoves[i * 9 + j] = mv; 						
				}
			}
		}
	
		void Update(int *inBoard){
			for(int y = 0; y < 9; y++){
				for(int x = 0; x < 9; x++){
					int index = CTI(x,y);
					int oldValue = board[index];
					int pV = inBoard[x + y * 9];
					int newValue = (pV == 0) ? ND : ((pV == 1) ? X : O);
					
					if(oldValue != newValue){
						movesMade++;
						int macroMove = index % 9;
						board[index] = newValue;
						
						boardHash[index / 9] += hash_tts[newValue][macroMove];

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
			fprintf(stderr, "%d\n", gl_PrevMove);
			printer->Print(board);
		}

		std::pair<int, int> Move(int time){
			if(movesMade == 0){
				return std::make_pair(4, 4);	
			}
			struct timeval tp;
			gettimeofday(&tp, NULL);
			long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
			float sec = 0.0;
			
			transHitFull = 0;
			transHit = 0;
			mNodes = 0;
			qNodes = 0;
			LMR_Re = 0;
			LMR_NONE = 0;
			
			for(int j = 0; j < 9; j++){
				uint64_t v = ticTacEval->eval(board + j * 9);
				mBoard[j] = v & ticTacEval->BM_EVAL;
				mBoardFull[j] = v; 
			}	
				
			int depth;
			for(depth = 4; depth < 25; depth++){
				itDepth = depth;
				auto eval = Negamax(gl_PrevMove, depth, CONST_WON - 100, -CONST_WON + 100, 1, 0);
				
				gettimeofday(&tp, NULL);
				long int ms2 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
				sec = ((float) (ms2-ms) / 1000.0);
				fprintf(stderr, "%d - %f - %f - %d\n", depth, eval, sec, mNodes);
				if(time < 6500 && sec > 0.250)
					break;
				if(time < 5000 && sec > 0.150)
					break;
				if(time < 2500 && sec > 0.100)
					break;
				// if(sec > 0.250 && movesMade < 14 || sec > 0.450)
				if(sec > 0.350)
					break;
			}
		
			hashKey->prevMove = gl_PrevMove % 9;
			hashKey->hash = hash ^ hash_move[gl_PrevMove % 9];
			auto transPos = transTable->find(hashKey);
			int chosenMove = -1;
			float move = 0.0;

			if(transPos != transTable->end()){
				chosenMove = transPos->second->bestMove;
				move = transPos->second->eval;
			}else{
				fprintf(stderr, "Error, chosen move not found in transposition table, we are fucked\n");
			}

			if(chosenMove == -1){
				int k = 0, kk = 0;
				mover->getMoves(movePool, gl_PrevMove, pl, &k, -1, &kk, 1);
				chosenMove = movePool[0];
				fprintf(stderr, "Choosing loose %d\n", chosenMove);
			}

			fprintf( stderr, "%d: Depth %d: %2.3f mN/sec - %2.3f sec\n", movesMade, depth, ((float) mNodes / 1000000.0) / sec, sec);
			fprintf( stderr, "TransHit: %d Full: %d, transsize: %ld \n", transHit, transHitFull, transTable->size());
			fprintf( stderr, "Nodes: %d qNodes: %d LMR_Re: %d - %d\n", mNodes, qNodes, LMR_Re, LMR_NONE);
			
			if(move < CONST_WON || move > -CONST_WON){
				fprintf( stderr, "Best move: %d, %f # ?? \n", chosenMove, move);
			}else{
				fprintf( stderr, "Best move: %d, %f\n", chosenMove, move);
			}

			// for(int i = 0; i < 21; i++){
			// 	fprintf(stderr, "%d: %d\n", i, pearsonValues[i]);
			// }

			int macro = chosenMove / 9;
			int micro = chosenMove % 9;
			int x = (macro % 3) * 3 + micro % 3;
			int y = (macro / 3) * 3 + micro / 3;
		
			fprintf( stderr, "%d, %d\n", x,y);
			transTable->cleanUp(movesMade + 1);
			return std::make_pair(x, y);	
		}
		
		void Ponder(){
		}

		void SetPlayer(char pl){
			this->pl = pl;
		  op = (pl == 1) ? 2 : 1;  
		}

	private:
		float Negamax(int prevMove, int depth, float alpha, float beta, int turn, int shallow){
			mNodes++;
			int token = (turn == 1) ? pl : op;
			
			//Check transtable
			hashKey->prevMove = prevMove % 9;
			hashKey->hash = hash ^ hash_move[prevMove % 9];
			auto transPos = transTable->find(hashKey);

			char prefMove = -1;

			if(transPos != transTable->end()) {		
				prefMove = transPos->second->bestMove;
				transHit++;
				if(transPos->second->valDepth == itDepth + depth && transPos->second->shallow == 0){
					transHitFull++;
					return transPos->second->eval;
				}
			}
			
			int qMoves = 0, wcMoves = 0;
			int* moves = &movePool[82 * (80 - (depth <= 0 ? 0 : depth))];
			mover->getMoves(moves, prevMove, token, &qMoves, prefMove, &wcMoves, turn);
			
			if(depth <= 0){
				if(qMoves == 1){
					return qSearch(prevMove, turn, 0, alpha, beta, -1);
				}
				return evaluater->H(board, mBoardFull, pl, ticTacEval) * turn;
			}

			if(moves[0] == 999){
				return 0.0;
			}

			// std::vector<std::pair<float,int>> *pearson;
			// int pear = 0;
			// if(itDepth - depth < 3){
			// 	pear = 1;
			// 	pearson = new std::vector<std::pair<float,int>>();
			// }

			float extreme = -INFINITY;
			int chosenMove = -1;
			int c = 0;

			for(int i = -1;; ){
				i = moves[c++];
				if(i == 999){
					break;
				}

				auto hashChanges = ApplyMove(i, token);
				auto term = std::get<0>(hashChanges);

				float e;
				
				auto wonLost = term & TicTacEval::BM_EVAL;
				auto term2 = std::get<0>(hashChanges);
				if(wonLost != 0 && wonLost != 3){
					e = -CONST_WON + depth;
				}else if(wonLost == 3 || ((((term >> (TicTacEval::opb + 18)) & TicTacEval::BM_EVAL) == 0) && 
																	(((term >> (TicTacEval::plb + 18)) & TicTacEval::BM_EVAL) == 0))
															|| ((((term2 >> (TicTacEval::opb + 18)) & TicTacEval::BM_EVAL) == 0) && 
																	(((term2 >> (TicTacEval::plb + 18)) & TicTacEval::BM_EVAL) == 0))){
					e = 0.0;
				}else{
					if (c != 1){
						e = -Negamax(i, depth-1, -alpha-1, -alpha, -turn, 1);
	          if (alpha < e && e < beta){
	         		e = -Negamax(i, depth-1, -beta, -e, -turn, shallow);
	          }
	        }
	        else{
	          e = -Negamax(i, depth-1, -beta, -alpha, -turn, shallow);	
	        }
				}
			
				RemoveMove(i, token, hashChanges);

				if(e > extreme){
					extreme = e;
					chosenMove = i;
				}

				alpha = std::max(e, alpha);

				if(alpha >= beta){
					break;
				}
			}

			if(itDepth - depth < hashDepth){
				if(transPos != transTable->end()){
					TranspositionTable::UpdateTransPos(transPos, chosenMove, extreme, itDepth + depth, shallow);
				}else{
					transTable->insert(chosenMove, hash ^ hash_move[prevMove%9], extreme, itDepth + depth, movesMade, shallow, board, prevMove);
				}
			}
			return extreme;
		} 

		float qSearch(int prevMove, int turn, int depth, float alpha, float beta, char allowFinish){
			mNodes++;
			qNodes++;
			int token = (turn == 1) ? pl : op;	

			int qMoves = 0, wcMoves = 0;
			int* moves = movePoolQ + (28 - depth) * 81;

			mover->getMoves(moves, prevMove, token, &qMoves, -1, &wcMoves, turn);

			if(qMoves != 1 && allowFinish){
				return evaluater->H(board, mBoardFull, pl, ticTacEval) * turn;
			}

			float extreme = -INFINITY;
			for(int i = 0, c = 0; c < qMoves && i != 999; ){
				i = moves[c++];
				if(i == 999){
					return 0.0;
				}

				auto hashChanges = ApplyMove(i, token);
				auto term = std::get<0>(hashChanges);

				float	e;

				auto wonLost = term & TicTacEval::BM_EVAL;
				auto term2 = std::get<5>(hashChanges);
				if(wonLost != 0 && wonLost != 3){
					e = -CONST_WON + depth;
				}else if(wonLost == 3 || ((((term >> (TicTacEval::opb + 18)) & TicTacEval::BM_EVAL) == 0) && 
																	(((term >> (TicTacEval::plb + 18)) & TicTacEval::BM_EVAL) == 0))
															|| ((((term2 >> (TicTacEval::opb + 18)) & TicTacEval::BM_EVAL) == 0) && 
																	(((term2 >> (TicTacEval::plb + 18)) & TicTacEval::BM_EVAL) == 0))){
					e = 0.0;
				}{
					e = -qSearch(i, -turn, depth + 1, -beta, -alpha, -allowFinish);
				}

				RemoveMove(i, token, hashChanges);

				extreme = fmax(extreme, e);
				alpha = fmax(alpha, e);

				if(alpha >= beta){
					break;
				}
			}
			return extreme;
		}

		std::tuple<uint64_t, uint64_t, uint64_t, char, char, uint64_t> ApplyMove(int move, int token){
			int macroIndex = move / 9;
			board[move] = token;

			boardHash[macroIndex] += hash_tts[token][move % 9];
			uint64_t newMBoardValue = ticTacEval->evalPure(boardHash[macroIndex]);
			uint64_t oldMBoardValue = mBoardFull[macroIndex];
			mBoardFull[macroIndex] = newMBoardValue;
			mBoard[macroIndex] = newMBoardValue & ticTacEval->BM_EVAL;
			
			uint64_t hashChange = ((token == X) ? hash_pl[move] : hash_op[move]);
			hash = hash ^ hashChange;
			
			//Terminal position
			uint64_t term = ticTacEval->eval(mBoard);
			uint64_t term2 = ticTacEval->evalPl(mBoard,1);
			uint64_t term3 = ticTacEval->evalPl(mBoard,2);
			char oldPlWin = plCanWinMap[macroIndex];
			char oldOpWin = opCanWinMap[macroIndex];
			plCanWinMap[macroIndex] = ((term >> (TicTacEval::plb + macroIndex * 2)) & TicTacEval::BM_EVAL) == 3;
			opCanWinMap[macroIndex] = ((term >> (TicTacEval::opb + macroIndex * 2)) & TicTacEval::BM_EVAL) == 3;

			return std::make_tuple(term, oldMBoardValue, hashChange, oldPlWin, oldOpWin, term2 | term3);	
		}

		void RemoveMove(int move, int token, std::tuple<uint64_t, uint64_t, uint64_t, char, char, uint64_t> changes){
			int macroIndex = move / 9;
			mBoardFull[macroIndex] = std::get<1>(changes);
			mBoard[macroIndex] = std::get<1>(changes) & ticTacEval->BM_EVAL;
			boardHash[macroIndex] -= hash_tts[token][move % 9];
			
			hash = hash ^ std::get<2>(changes);

			plCanWinMap[macroIndex] = std::get<3>(changes);
			opCanWinMap[macroIndex] = std::get<4>(changes);

			board[move] = 0;
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