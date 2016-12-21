#include <iostream>
#include <sys/time.h>
#include <math.h>


class Engine{
	char *board;
	char *mBoard;
	char *mBoardPool;
	char *movePool;
	char *lTmp;
	char *hTmp;
	int placeNext;
	char pl, op;
	int lMove, mNodes;
	int valSmall;
	int valSmallMove;
	int movesMade;
	const float mynan = 0.0/0.0;

	public:
		Engine(){
			board     	= (char*) malloc(sizeof(char) * 81);
			mBoardPool 	= (char*) malloc(sizeof(char) * 9 * 81);
			movePool 	  = (char*) malloc(sizeof(char) * 81 * 81);
			lTmp 		  	= (char*) malloc(sizeof(char) * 9);
			hTmp 		  	= (char*) malloc(sizeof(char) * 9);
			for(int i = 0; i < 9 * 81; i++){
				mBoardPool[i] = 0;
			}
			placeNext  	= -1;
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
							placeNext = index % 9;
						}
					}
				}
			}	
			this->Print();
		}

		std::pair<int, int> Move(int time){
			struct timeval tp;
			int macroIndex = placeNext * 9;
			int bestMove = -1;

			int minLim = 0, maxLim = 81;

			if(placeNext != -1 && this->ValidateSmall(board + macroIndex) == 0){ //Strict macro placement
				minLim = macroIndex;
				maxLim = minLim + 9;
			}

			gettimeofday(&tp, NULL);
			long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
			float sec = 0.0;
			int depth;
			
			float extreme = -INFINITY;

			for(depth = 2; depth < 82-movesMade; depth++){ //
				mNodes = 0;
				valSmall = 0;
				valSmallMove = 0;

				extreme = -INFINITY;
				float alpha = -INFINITY;
				float beta = INFINITY;

				char* lmBoard = mBoardPool;
				char* rmBoard = mBoardPool + 9;

				for(int j = 0; j < 9; j++){
					int v = ValidateSmall(board + j * 9);
					lmBoard[j] = v;
				}							

				for(int i = minLim; i < maxLim; i++){
					if(lmBoard[i / 9] != 0){
						i += 8;
						continue; //Skipping macros that is done
					} 

					if(board[i] == 0){
						if(bestMove == -1){
							bestMove = i;
						}
						board[i] = pl;
						
						for(int j = 0; j < 9; j++){
							rmBoard[j] = lmBoard[j];
						}		
						rmBoard[i / 9] = ValidateSmall(board + (i - i % 9));

						float e = Min(rmBoard, i, depth, alpha, beta);
						board[i] = 0;

						if(isnan(e) && bestMove == -1){
							bestMove = i;
						}
						if(e >= extreme){
							extreme = e;
							bestMove = i;
						}
						if(extreme >= beta){
							if(bestMove == -1){
								bestMove = i;
							}
							break;
						}
						if(extreme > alpha){
							alpha = extreme;
						}
					}
				}

				if(bestMove == -1){
					fprintf(stderr, "%d - %d - %d\n", minLim, maxLim, macroIndex);
				}

				gettimeofday(&tp, NULL);
				long int ms2 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
				sec = ((float) (ms2-ms) / 1000.0);
				if(time < 5000 && sec > 0.200){
					break;
				}if(time < 2500 && sec > 0.120){
					break;
				}
				if(sec > 0.310){
					break;
				}
			}

			fprintf( stderr, "%d: Depth %d: %2.3f mN/sec - %2.3f sec\n", movesMade, depth, ((float) mNodes / 1000000.0) / sec, sec);
			fprintf( stderr, "Best move: %d, %f\n", bestMove, extreme);

			int macro = bestMove / 9;
			int micro = bestMove % 9;
			int x = (macro % 3) * 3 + micro % 3;
			int y = (macro / 3) * 3 + micro / 3;

			fprintf( stderr, "%d, %d\n", x,y);
			//CONVERT MOVE BACK
			return std::make_pair(x, y);	
		}
		
		void SetPlayer(char pl){
			this->pl = (pl == 2) ? -1 : 1;
		  op = (pl == 1) ? -1 : 1;  
		  fprintf( stderr,"Player is %d\n", pl);
		}

	private:
		float Min(char* lmBoard, int move, int depth, float alpha, float beta){
			mNodes++;
			char v = this->ValidateBig(lmBoard);

			if(v != 0 && v != -2){
				return INFINITY;
			}else if(v == -2){
				return 0.0;
			}else if(depth == 0){
				return H(lmBoard);
			}

			int minLim;
			int maxLim;
			float extreme = INFINITY;

			getLimit(&minLim, &maxLim, move, lmBoard);

			char* rmBoard = mBoardPool + (depth + 1) * 9;

			for(int i = minLim; i < maxLim; i++){
				if(lmBoard[i / 9] != 0){
					i += 8;
					continue; //Skipping macros that is done
				} 

				if(board[i] == 0){
					board[i] = op;
					
					for(int j = 0; j < 9; j++){
						rmBoard[j] = lmBoard[j];
					}		
					rmBoard[i / 9] = ValidateSmall(board + (i - i % 9));

					float e = Max(rmBoard, i, depth - 1, alpha, beta);
					board[i] = 0;

					if(e <= extreme){
						extreme = e;
					}
					if(e <= alpha){
						return mynan;
					}
					if(e < beta){
						beta = e;
					}
				}
			}
			return extreme;
		} 

		float Max(char* lmBoard, int move, int depth, float alpha, float beta){
			mNodes++;
			char v = this->ValidateBig(lmBoard);

			if(v != 0 && v != -2){
				return -INFINITY;
			}else if(v == -2){
				return 0.0;
			}else if(depth == 0){
				return H(lmBoard);
			}

			int minLim;
			int maxLim;
			float extreme = -INFINITY;

			getLimit(&minLim, &maxLim, move, lmBoard);

			char* rmBoard = mBoardPool + (depth + 1) * 9;

			for(int i = minLim; i < maxLim; i++){
				if(lmBoard[i / 9] != 0){
					i += 8;
					continue; //Skipping macros that is done
				} 

				if(board[i] == 0){
					board[i] = pl;
					
					for(int j = 0; j < 9; j++){
						rmBoard[j] = lmBoard[j];
					}		
					rmBoard[i / 9] = ValidateSmall(board + (i - i % 9));

					float e = Min(rmBoard, i, depth - 1, alpha, beta);
					board[i] = 0;

					if(e >= extreme){
						extreme = e;
					}
					if(e >= beta){
						return mynan;
					}
					if(e > alpha){
						alpha = e;	
					}
				}
			}

			return extreme;
		} 

		void getLimit(int *minLim, int *maxLim, int move, char* lmBoard){
			int macroIndex = move % 9;
			*minLim = 0;
			*maxLim = 81;

			if(lmBoard[macroIndex] == 0){ //Strict macro placement
				*minLim = macroIndex * 9;
				*maxLim = *minLim + 9;
			}
		}

		float H(char *lmBoard){
			float centH = 0.0;

			for(int mac = 0; mac < 9; mac++){
				if(lmBoard[mac] != 0){
					continue;
				}
				float macFac = 0.02;
				if(mac % 2 == 1){
					if(mac == 4){
						macFac = 0.08;
					}else{
						macFac = 0.015;
					}
				}
				for(int mic = 0; mic < 9; mic++){
					float micFac = 0.002;
					if(mic % 2 == 1){
						if(mic == 4){
							micFac = 0.008;
						}else{
							micFac = 0.0015;
						}
					}	
					centH += macFac * micFac * board[mac * 9 + mic];
				}
			}

			for(int i = 0; i < 9; i++){
				lTmp[i] = 0; 
				hTmp[i] = 0; 

				int t = lmBoard[i];

				if(t == -2){
					continue;
				}
				int iT = t * -1;

				int pred = 9;
				int base = t - t % 3;
				for(int j = 0; j < 3; j++){
					int v = lmBoard[base + j];
					if(v == iT){
						pred = 0;
						break;
					}else if(v == 0){
						pred -= 3;
					}
				}
				if(pred > lTmp[i]){
					lTmp[i] = pred;
				}

				pred = 9;
				base = t - (t / 3) * 3;
				for(int j = 0; j < 3; j++){
					int v = lmBoard[base + j * 3];
					if(v == iT){
						pred = 0;
						break;
					}else if(v == 0){
						pred -= 3;
					}
				}

				if(pred > lTmp[i]){
					lTmp[i] = pred;
				}

				if(t == 0 || t == 4 || t == 8){
					pred = 9;
					for(int j = 0; j < 3; j++){
						int v = lmBoard[j * 4];
						if(v == iT){
							pred = 0;
							break;
						}else if(v == 0){
							pred -= 3;
						}
					}	
					if(pred > lTmp[i]){
						lTmp[i] = pred;
					}
				}

				if(t == 2 || t == 4 || t == 6){
					pred = 3;
					for(int j = 0; j < 3; j++){
						int v = lmBoard[2 + j * 4];
						if(v == iT){
							pred = 0;
							break;
						}else if(v == 0){
							pred -= 3;
						}
					}	
					if(pred > lTmp[i]){
						lTmp[i] = pred;
					}
				}
				lTmp[i] *= t;
			}

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
				h += hTmp[i] * lTmp[i] + lTmp[i];
			}
			return (h +centH) * pl;
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
					l = v;
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
					l = v;
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
				l = v;
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
				l = v;
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

		char ValidateBig(char *mB){
			return this->ValidateSmall(mB);
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
			fprintf( stderr, (" |-----------|\n\n"));
		}
};