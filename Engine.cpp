#include <iostream>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include "HashBoard.cpp"
#include "TranspositionTable.cpp"

class Engine{
	const float mynan = 0.0/0.0;

	char *board;
	char *mBoard;
	char *pBoard;
	char *mBoardPool;
	int *movePool;
	char *plTmp;
	char *opTmp;
	char *hTmp;
	int gl_PrevMove;
	char pl, op;
	int lMove, mNodes;
	int movesMade;
	int hash = 0;
	int itDepth = 0;
	int transHitFull = 0;
	int transHit = 0;
	int hashDepth = 12;

	TranspositionTable* transTable;
	HashBoard* hashKey;

	public:
		Engine(){
			board     	= (char*) malloc(sizeof(char) * 81);
			mBoardPool 	= (char*) malloc(sizeof(char) * 9 * 81);
			movePool 	  = new int[83 * 81];
			plTmp 		  = (char*) malloc(sizeof(char) * 9);
			opTmp 		  = (char*) malloc(sizeof(char) * 9);
			hTmp 		  	= (char*) malloc(sizeof(char) * 9);
			pBoard 		  = (char*) malloc(sizeof(char) * 9);
			transTable  = new TranspositionTable();
			hashKey 		= new HashBoard();
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
					int newValue = (pV == 0 ? 0 : (pV == 1) ? 1 : -1);
					if(newValue != 0){
						movesMade++;
					}
					if(oldValue != newValue){
						board[index] = newValue;
						
						if(newValue != pl){
							gl_PrevMove = CTI(x,y);
							hash = hash ^ hash_op[index];
						}else{
							hash = hash ^ hash_pl[index]; 
						}
					}
				}
			}	
			// Print();
		}

		std::pair<int, int> Move(int time){
			struct timeval tp;
			gettimeofday(&tp, NULL);
			long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
			float sec = 0.0;
			
			transHitFull = 0;
			transHit = 0;
			mNodes = 0;

			char* lmBoard = mBoardPool;
 			
			for(int j = 0; j < 9; j++){
				int v = ValidateSmall(board + j * 9);
				lmBoard[j] = v;
			}	

			int depth;
			for(depth = 2; depth < 35; depth++){
				itDepth = depth;
				
				Max(lmBoard, gl_PrevMove, depth, -INFINITY, INFINITY);
				
				gettimeofday(&tp, NULL);
				long int ms2 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
				sec = ((float) (ms2-ms) / 1000.0);
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
			fprintf( stderr, "Best move: %d, %f\n", chosenMove, transPos->first->eval);

			int macro = chosenMove / 9;
			int micro = chosenMove % 9;
			int x = (macro % 3) * 3 + micro % 3;
			int y = (macro / 3) * 3 + micro / 3;

			fprintf( stderr, "%d, %d\n", x,y);
			transTable->cleanUp(movesMade + 2); //Should be done on pondering.
			return std::make_pair(x, y);	
		}
		
		void SetPlayer(char pl){
			this->pl = (pl == 2) ? -1 : 1;
		  op = (pl == 1) ? -1 : 1;  
		}

	private:
		float Min(char* lmBoard, int prevMove, int depth, float alpha, float beta){
			mNodes++;
			char v = this->ValidateSmall(lmBoard);

			hashKey->prevMove = prevMove % 9;
			hashKey->hash = hash;
			
			auto transPos = transTable->find(hashKey);
			char prefMove = -1;
			if(transPos != transTable->end()) {
				prefMove = transPos->second;
				transHit++;
				if(transPos->first->itDepth == itDepth && !isnan(transPos->first->eval)){
					transHitFull++;
					return transPos->first->eval;
				}
			}

			if(v != 0 && v != -2){
				return INFINITY;
			}else if(v == -2){
				return 0.0;
			}else if(depth == 0){
				return H(lmBoard);
			}

			float extreme = INFINITY;

			char* rmBoard = mBoardPool + (depth + 1) * 9 * sizeof(char);				

			int* moves = &movePool[83 * (80 - depth)];
			getMoves(moves, prevMove, lmBoard, prefMove);

			int chosenMove = 0;
			for(int i = 0, c = 0; ;){
				i = moves[c++]; 
				if(i == -1){
					fprintf(stderr, "ERROR\n");
				}
				if(i == 999){
					break;
				} 
				
				memcpy(rmBoard, lmBoard, 9 * sizeof(char));				

				int hashChange = hash_op[i];
				board[i] = op;
				rmBoard[i / 9] = ValidateSmall(board + (i - i % 9));
				hash = hash ^ hashChange;
				float e = Max(rmBoard, i, depth - 1, alpha, beta);
				hash = hash ^ hashChange;
				board[i] = 0;

				if(e <= extreme){
					extreme = e;
					chosenMove = i;
				}
				if(e <= alpha){
					extreme = mynan;
					chosenMove = i;
					break;
				}
				if(e < beta){
					beta = e;
				}
			}

			if(itDepth - depth < hashDepth){
				if(transPos != transTable->end()){
					TranspositionTable::UpdateTransPos(transPos, chosenMove, extreme, itDepth);
				}else{
					transTable->insert(chosenMove, board, hash, extreme, prevMove, itDepth, movesMade + (itDepth - depth));
				}
			}

			return extreme;
		} 

		float Max(char* lmBoard, int prevMove, int depth, float alpha, float beta){
			mNodes++;
			char v = this->ValidateSmall(lmBoard);
			hashKey->prevMove = prevMove % 9;
			hashKey->hash = hash;
			auto transPos = transTable->find(hashKey);
			
			char prefMove = -1;
			if(transPos != transTable->end()) {
				prefMove = transPos->second;
				transHit++;
				if(transPos->first->itDepth == itDepth && !isnan(transPos->first->eval)){
					transHitFull++;
					return transPos->first->eval;
				}
			}

			if(v != 0 && v != -2){ return -INFINITY;}
			else if(v == -2){ return 0.0;}
			else if(depth == 0){ return H(lmBoard);}

			char* rmBoard = mBoardPool + (depth + 1) * 9 * sizeof(char);				

			int* moves = &movePool[83 * (80 - depth)];
			getMoves(moves, prevMove, lmBoard, prefMove);

			float extreme = -INFINITY;
			int chosenMove = 0;
			for(int i = 0, c = 0; ;){
				i = moves[c++]; 
				if(i == -1){
					fprintf(stderr, "ERROR\n");
				}
				if(i == 999){break;} 
				
				memcpy(rmBoard, lmBoard, 9 * sizeof(char));				

				int hashChange = hash_pl[i];
				board[i] = pl;
				rmBoard[i / 9] = ValidateSmall(board + (i - i % 9));
				hash = hash ^ hashChange;
				float e = Min(rmBoard, i, depth - 1, alpha, beta);
				hash = hash ^ hashChange;
				board[i] = 0;

				if(e >= extreme){
					extreme = e;
					chosenMove = i;
				}
				if(e >= beta){
					extreme = mynan;
					chosenMove = i;
					break;
				}
				if(e > alpha){
					alpha = e;	
				}
			}
			
			if(itDepth - depth < hashDepth){
				if(transPos != transTable->end()){
					TranspositionTable::UpdateTransPos(transPos, chosenMove, extreme, itDepth);
				}else{
					transTable->insert(chosenMove, board, hash, extreme, prevMove, itDepth, movesMade + (itDepth - depth));
				}
			}

			return extreme;
		} 

		void getMoves(int* moves, int prevMove, char* lmBoard, int prefMove){
			int macroIndex = prevMove % 9;
			int minLim = 0;
			int maxLim = 81;

			if(prevMove > -1 && lmBoard[macroIndex] == 0){ //Strict macro placement
				minLim = macroIndex * 9;
				maxLim = minLim + 9;
			}
			
			int lCount = 0, hCount = 81, prefIndex = -1;
			
			for(int i = minLim; i < maxLim; i++){	
				if(lmBoard[i / 9] != 0){
					i += 8;
					continue; //Skipping macros that is done
				} 
				if(board[i] == 0){
					if(lmBoard[i] == 0){
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
			for(int i = 0; i < 9; i++){
				plTmp[i] = 0; 
				opTmp[i] = 0; 
				hTmp[i] = 0; 
			}

			for(int i = 0; i < 3; i++){
				hDirection(lmBoard, i * 3, 1); //Horizontal
				hDirection(lmBoard, i, 3); //Vertical
			}
			hDirection(lmBoard, 0, 4);
			hDirection(lmBoard, 2, 2);

			for(int i = 0; i < 9; i++){
				if(lmBoard[i] == 0){
					for(int j = 0; j < 9; j++){
						if(board[i * 9 + j] == 0){
							hTmp[j] += 1;
						}
					}	
				}
			}

			float h = 0.0;
			for(int i = 0; i < 9; i++){
				h += hTmp[i] * plTmp[i] + plTmp[i] * 4;
				h += hTmp[i] * opTmp[i] + opTmp[i] * 4;
			}
			return h * pl;
		}

		void hDirection(char *lmBoard, int base, int step){
			int o = 0;
			int m = 0;
			int c = 1;

			for(int j = 0; j < 3; j++){
				int v = lmBoard[base + j * step];
				if(v == -2){
					c=0;
					break;
				}
				if(v != 0){
					if(o == 0){
						o = v;
						m += 1 << j;
					}else if(v != o){
						c=0;
						break;
					}else{
						c+=1;
					}
				}
			}
			if(c == 2){
				for(int j = 0; j < 3; j++){
					if((m & (1 << j)) == 0){
						int ind = base + j * step;
						int plV = plTmp[ind];
						int opV = opTmp[ind];
						int nV = c * o;
						if(plV < nV){
							plTmp[ind] = nV;	
						}  
						if(opV > nV){
							opTmp[ind] = nV;	
						} 
					}
				}
			}
		}

		char ValidateSmall(char *b){
			char l, v;

			//Horizontal
			for(int i = 0; i < 3; i++){
				l = b[i * 3];
				for(int j = 0; j < 3; j++){
					v = b[i * 3 + j];	
					if(l != v || v == 0){
						v = 0;
						break;
					}
				}
				if(v != 0){
					return v;
				}
			}
			
			//Vertical
			for(int i = 0; i < 3; i++){
				l = b[i];
				for(int j = 0; j < 3; j++){
					v = b[i + j * 3];
					if(l != v || v == 0){
						v = 0;
						break;
					}
				}
				if(v != 0){
					return v;
				}
			}
			
			//diag
			l = b[0];
			for(int i = 0; i < 3; i++){
				v = b[i * 4];
				if(v == 0 || l != v){
					v = 0;
					break;
				}
			}
			if(v != 0){
				return v;
			}

			l = b[2];
			for(int i = 0; i < 3; i++){
				v = b[2 + i * 2];
				if(v == 0 || l != v){
					v = 0;
					break;
				}
			}
			if(v != 0){
				return v;
			}

			for(int i = 0; i < 9; i++){
				if(b[i] == 0){
					return 0;
				}
			}

			return -2;
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
						if(p == -1){ c = 'O';}
						if(p == 1){ c = 'X';}
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
						if(p == -1){ c = 2;}
						if(p == 1){ c = 1;}
						fprintf( stderr, "%d,", c);
					}
				}
			}
			fprintf( stderr, (" ]\n\n"));
		}
};