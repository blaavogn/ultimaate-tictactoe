#include <iostream>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "HashBoard.cpp"
#include "TranspositionTable.cpp"
#include "TicTacEval.cpp"

class Engine{
	const float mynan = 0.0/0.0 * -1;
	
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	const uint64_t BM_FULL = 0xffffffffffffffff;
	const uint64_t BM_EVAL = (BM_FULL & ((1 << 2) - 1));
	const uint64_t BM_PL   = (BM_FULL & ((1 << 20) - 1)) ^ BM_EVAL;
	const uint64_t BM_OP   = (BM_FULL & (((uint64_t)1 << 39) - 1)) ^ BM_PL;

	char *board;
	char *mBoard;
	char *pBoard;
	char *mBoardPool;
	int *movePool;
	char *tmpBoard;
	int gl_PrevMove;
	char pl, op;
	int lMove, mNodes;
	int movesMade;
	int hash = 0;
	int itDepth = 0;
	int transHitFull = 0;
	int transHit = 0;
	int hashDepth = 10;

	float gl_Eval = 0.0;
	TranspositionTable* transTable;
	TicTacEval* ticTacEval;
	HashBoard* hashKey;

	public:
		Engine(){
			board     	= (char*) malloc(sizeof(char) * 81);
			mBoardPool 	= (char*) malloc(sizeof(char) * 9 * 81);
			movePool 	  = new int[82 * 81];
			tmpBoard 		  = (char*) malloc(sizeof(char) * 9);
			pBoard 		  = (char*) malloc(sizeof(char) * 9);
			transTable  = new TranspositionTable();
			hashKey 		= new HashBoard();
			ticTacEval  = new TicTacEval();
			hashKey->board = board;
			gl_PrevMove  	= -1;
			movesMade   = 0;
		}
	
		void Update(int *inBoard){
			movesMade = 0;
			for(int y = 0; y < 9; y++){
				for(int x = 0; x < 9; x++){
					int index = this->CTI(x,y);
					int oldValue = board[index];
					int pV = inBoard[x + y * 9];
					if(pV != 0){
						movesMade++;
					}
					int newValue = (pV == 0) ? ND : ((pV == 1) ? X : O);
					if(oldValue != newValue){
						board[index] = newValue;
						if(pV != pl){
							gl_PrevMove = CTI(x,y);
							hash = hash ^ hash_op[index];
						}else{
							hash = hash ^ hash_pl[index]; 
						}
					}
				}
			}	
			Print();
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

			char* lmBoard = mBoardPool;
			char* rmBoard = mBoardPool + 9 * sizeof(char);
 			
			for(int j = 0; j < 9; j++){
				int v = ValidateMacro(board + j * 9) & BM_EVAL;
				lmBoard[j] = v;
			}	
				
			int depth;
			for(depth = 1; depth < 25; depth++){
				itDepth = depth;

				memcpy(rmBoard, lmBoard, 9 * sizeof(char));

				gl_Eval = MTDF(gl_Eval, rmBoard, gl_PrevMove, depth);
				

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
			fprintf( stderr, "%d: Depth %d: %2.3f mN/sec - %2.3f sec\n", movesMade, depth - 1, ((float) mNodes / 1000000.0) / sec, sec);
			fprintf( stderr, "TransHit: %d Full: %d, transsize: %ld \n", transHit, transHitFull, transTable->size());
			fprintf( stderr, "Best move: %d, %f\n", chosenMove, transPos->first->eval);

			int macro = chosenMove / 9;
			int micro = chosenMove % 9;
			int x = (macro % 3) * 3 + micro % 3;
			int y = (macro / 3) * 3 + micro / 3;

			fprintf( stderr, "%d, %d\n", x,y);
			transTable->cleanUp(movesMade + 1); //Should be done on pondering.
			return std::make_pair(x, y);	
		}
		
		void SetPlayer(char pl){
			this->pl = pl;
		  op = (pl == 1) ? 2 : 1;  
		}

	private:
		float MTDF(float f, char* lmBoard, int prevMove, int depth){
      float g = 0.0;
      float upperBound = INFINITY;
      float lowerBound = -INFINITY;
      while (lowerBound < upperBound){
    		float beta = (g > lowerBound+1) ? g : lowerBound+1;
    		g = Max(lmBoard, prevMove, depth, beta-1, beta);
  		 	if (g < beta){
          upperBound = g;
  		 	}
       	else{
          lowerBound = g;
       	}
      }
     return g;
    }

		float Max(char* lmBoard, int prevMove, int depth, float alpha, float beta){
			
			mNodes++;
			char v = ValidateMacroDelta(lmBoard, prevMove / 9);
			hashKey->prevMove = prevMove % 9;
			hashKey->hash = hash;
			auto transPos = transTable->find(hashKey);
			char prefMove = -1;
			
			if(transPos != transTable->end()) {
				prefMove = transPos->second;
				transHit++;
				if(transPos->first->itDepth == itDepth && !transPos->first->cut){
					transHitFull++;
					return transPos->first->eval;
				}
			}

			if(v != ND && v != DR){
				return -INFINITY;
			}
			else if(depth == 0){
				 if(v == DR || v == ND){
					return 0.0;
				}
				return H(lmBoard);
			}

			char* rmBoard = mBoardPool + (depth + 1) * 9 * sizeof(char);				

			int* moves = &movePool[82 * (80 - depth)];
			getMoves(moves, prevMove, lmBoard, prefMove);
			
			float extreme = -INFINITY;
			int chosenMove = 0;
			char cut = 0;
			for(int i = 0, c = 0; ;){
				i = moves[c++]; 
				if(i == 999){
					if(c == 1){
						cut = 2;
					}
					break;
				} 
				memcpy(rmBoard, lmBoard, 9 * sizeof(char));				

				int hashChange = hash_pl[i];
				board[i] = pl;
				rmBoard[i / 9] = ValidateMicro(board + (i - i % 9)) & BM_EVAL;
				hash = hash ^ hashChange;
				float e = Min(rmBoard, i, depth - 1, alpha, beta);
				hash = hash ^ hashChange;
				board[i] = 0;

				if(e >= extreme){
					extreme = e;
					chosenMove = i;
				}
				if(e >= beta){
					chosenMove = i;
					cut = 1;
					break;
				}
				if(e > alpha){
					alpha = e;	
				}
			}
			if(cut == 2){
				extreme = 0.0;
				cut = 0;
			}
			if(itDepth - depth < hashDepth){
				if(transPos != transTable->end()){
					TranspositionTable::UpdateTransPos(transPos, chosenMove, extreme, itDepth, cut);
				}else{
					transTable->insert(chosenMove, board, hash, extreme, prevMove, itDepth, movesMade + (itDepth - depth), cut);
				}
			}
			return extreme;
		} 

		float Min(char* lmBoard, int prevMove, int depth, float alpha, float beta){
			mNodes++;
			char v = ValidateMacroDelta(lmBoard, prevMove / 9);

			hashKey->prevMove = prevMove % 9;
			hashKey->hash = hash;
			
			auto transPos = transTable->find(hashKey);
			char prefMove = -1;
			if(transPos != transTable->end()) {
				prefMove = transPos->second;
				transHit++;
				if(transPos->first->itDepth == itDepth && !transPos->first->cut){
					transHitFull++;
					return transPos->first->eval;
				}
			}

			if(v != ND && v != DR){
				return INFINITY;
			}
			else if(depth == 0){
				 if(v == DR || v == ND){
					return 0.0;
				}
				return H(lmBoard);
			}

			float extreme = INFINITY;

			char* rmBoard = mBoardPool + (depth + 1) * 9 * sizeof(char);				

			int* moves = &movePool[82 * (80 - depth)];
			getMoves(moves, prevMove, lmBoard, prefMove);

			char cut = 0;
			int chosenMove = 0;
			for(int i = 0, c = 0; ;){
				i = moves[c++]; 
				if(i == 999){
					if(c == 1){
						cut = 2;
					}
					break;
				} 
				
				memcpy(rmBoard, lmBoard, 9 * sizeof(char));				

				int hashChange = hash_op[i];
				board[i] = op;
				rmBoard[i / 9] = ValidateMicro(board + (i - i % 9)) & BM_EVAL;
				hash = hash ^ hashChange;
				float e = Max(rmBoard, i, depth - 1, alpha, beta);
				hash = hash ^ hashChange;
				board[i] = 0;

				if(e <= extreme){
					extreme = e;
					chosenMove = i;
				}
				if(e <= alpha){
					chosenMove = i;
					cut = 1;
					break;
				}
				if(e < beta){
					beta = e;
				}
			}
			if(cut == 2){
				extreme = 0.0;
				cut = 0;
			}
			if(itDepth - depth < hashDepth){
				if(transPos != transTable->end()){
					TranspositionTable::UpdateTransPos(transPos, chosenMove, extreme, itDepth, cut);
				}else{
					transTable->insert(chosenMove, board, hash, extreme, prevMove, itDepth, movesMade + (itDepth - depth), cut);
				}
			}

			return extreme;
		} 

		void getMoves(int* moves, int prevMove, char* lmBoard, int prefMove){
			int macroIndex = prevMove % 9;
			int minLim = 0;
			int maxLim = 81;

			if(prevMove > -1 && lmBoard[macroIndex] == ND){ //Strict macro placement
				minLim = macroIndex * 9;
				maxLim = minLim + 9;
			}
			
			int lCount = 0, hCount = 81, prefIndex = -1;
			
			for(int i = minLim; i < maxLim; i++){	
				if(lmBoard[i / 9] != ND){
					i += 8;
					continue; //Skipping macros that is done
				} 
				if(board[i] == 0){
					if(lmBoard[i / 9] != DR){
						moves[lCount] = i;
						if(i == prefMove){
							prefIndex = lCount;				
						}
						lCount += 1;
					}else{
						moves[hCount] = i;
						hCount += 1;
					}
				}
			}
			
			while(hCount > 81){
				hCount -= 1;
				int i = moves[hCount];
				moves[lCount] = i;
				if(i == prefMove){
					prefIndex = lCount;				
				}
				lCount += 1;
			}
			if(prefIndex != -1){
				int t = moves[0];
				moves[0] = moves[prefIndex];
				moves[prefIndex] = t;
			}
			moves[lCount] = 999;					
		}

		float H(char *lmBoard){			
			float h = 0;
			uint64_t eval = ticTacEval->eval(lmBoard);

			for(int i = 0; i < 81; i++){
				if(board[i] == 0){
					tmpBoard[i % 9]++;
				}
			}
			for(int i = 0; i < 9; i++){
				h += (lmBoard[i] == DR ? 0 : (lmBoard[i] == X ? 1 : -1));
				int sh = i * 2;
				int plCon = (eval >> (sh + 20)) & BM_EVAL; 
				int opCon = -((eval >> (sh + 38)) & BM_EVAL); 
				int t = tmpBoard[i % 9];
				h += plCon * t + opCon * t; 
			}
			return h * (pl == X ? 1 : -1);
		}

		char ValidateMicro(char *b){
			int i = ticTacEval->eval(b) & BM_EVAL;
			return (i == 3) ? 3 : i;
		}

		uint64_t ValidateMacro(char *b){
			return ticTacEval->evalMacro(b);
		}

		char ValidateMacroDelta(char *b, int move){
			if(checkDir(b, move - move % 3, 1))
				return (b[move] == 1) ? X : O;
			if(checkDir(b, move - (move / 3) * 3, 3))
				return (b[move] == 1) ? X : O;

			if(move == 0 || move == 4 || move == 8){
				if(checkDir(b, 0, 4))
					return (b[move] == 1) ? X : O;
			}
			if(move == 2 || move == 4 || move == 6){
				if(checkDir(b, 2, 2))
					return (b[move] == 1) ? X : O;
			}

			return DR;
		}

		inline char checkDir(char *lmBoard, int base, int step){
			return lmBoard[base] != 0 && 
			       lmBoard[base] == lmBoard[base + step] && 
			       lmBoard[base] == lmBoard[base + 2 * step];
		}

		inline int CTI(int x, int y){
			int macroX = x / 3; 
			int macroY = (y / 3) * 3;
			int macro = macroX + macroY;

			int microX = x % 3; 
			int microY = y % 3 * 3; 
			int micro = microX + microY;
			return micro + macro * 9; 
		}


		void PrintMac(char* c){
			for(int i = 0; i < 9; i++){
				if(i % 3 == 0){
					fprintf(stderr, "\n");
				}
				fprintf(stderr, "%d,", c[i]);
			}
			fprintf(stderr, "\n");
		}

		void Print(){
			fprintf( stderr, "  012 345 678 \n");
			int row = 0;
			for(char k = 0; k < 9; k++){
				if(k%3 == 0){
					fprintf( stderr, " |-----------|\n%d|", row);
					row++;
				}
				for(char i = 0; i < 3; i++){
					for(char j = 0; j < 3; j++){
						char p = board[(k / 3) * 18 + k * 3 + i * 9 + j];

						char c = ' ';
						if(p == O){ c = 'O';}
						if(p == X){ c = 'X';}
						fprintf( stderr, "%c", c);
					}
					fprintf( stderr, "|");
				}
				if(k%3 != 2){
					fprintf( stderr, "\n%d|", row);
					row++;
				}else{
					fprintf( stderr, "\n");
				}
			}
			fprintf( stderr, (" |-----------|\n"));

			fprintf( stderr, " [");
			for(char k = 0; k < 9; k++){
				
				for(char i = 0; i < 3; i++){
					for(char j = 0; j < 3; j++){
						char p = board[(k / 3) * 18 + k * 3 + i * 9 + j];
						char c = 0;
						if(p == 2){ c = 2;}
						if(p == 1){ c = 1;}
						fprintf( stderr, "%d,", c);
					}
				}
			}
			fprintf( stderr, (" ]\n\n"));
		}
};