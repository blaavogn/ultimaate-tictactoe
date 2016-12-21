#include <iostream>
#include <sys/time.h>
#include <math.h>

class Engine{
	char *board;
	char *mBoard;
	char *mBoardPool;
	char *lTmp;
	int placeNext;
	char pl, op;
	int lMove, mNodes;
	int valSmall;
	int valSmallMove;
	int movesMade;
	
	public:
		Engine(){
			board     	= (char*) malloc(sizeof(char) * 81);
			mBoardPool 	= (char*) malloc(sizeof(char) * 9 * 81);
			lTmp 		  	= (char*) malloc(sizeof(char) * 9);
			for(int i = 0; i < 9 * 81; i++){
				mBoardPool[i] = 0;
			}
			placeNext  	= -1;
			movesMade   = 0;
		}
	
		void Update(int *inBoard){
			for(int y = 0; y < 9; y++){
				for(int x = 0; x < 9; x++){
					int index = this->CTI(x,y);
					int oldValue = board[index];
					int newValue = inBoard[x + y * 9];
					
					if(oldValue != newValue){
						board[index] = newValue;
						if(newValue != pl){
							placeNext = index % 9;
							//fprintf( stderr,"Next %d", placeNext);
						}
					}
				}
			}	
			movesMade++;
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
			
			float max = -INFINITY;
			for(int depth = 1; depth < 12; depth++){ //82-movesMade
				mNodes = 0;
				valSmall = 0;
				valSmallMove = 0;

				max = -INFINITY;

				char* lmBoard = mBoardPool;
				char* rmBoard = mBoardPool + 9;


				for(int j = 0; j < 9; j++){
					int v = ValidateSmall(board + j * 9);
					lmBoard[j] = v;
				}							

				for(int i = minLim; i < maxLim; i++){
					
					if(lmBoard[i / 9] != 0){
						i += 9;
						continue; //Skipping macros that is done
					} 
	
					if(board[i] == 0){
						board[i] = pl;
						
						for(int j = 0; j < 9; j++){
							rmBoard[j] = lmBoard[j];
						}		
						rmBoard[i / 9] = ValidateSmall(board + (i - i % 9));

						float e = MinMax(rmBoard, i, 0, depth, -1);
						printf("%f\n",e);
						if(e > max){
							max = e;
							bestMove = i;
						}
						board[i] = 0;
					}
				}

				gettimeofday(&tp, NULL);
				long int ms2 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
				float sec = ((float) (ms2-ms) / 1000.0);
				fprintf( stderr, "Depth %d: %2.3f mN/sec - %2.3f sec\n", depth, ((float) mNodes / 1000000.0) / sec, sec);
				if(sec > 1.400){
					break;
				}
			}

			fprintf( stderr, "Best move: %d, %3.4f\n", bestMove, max);

			int macro = bestMove / 9;
			int micro = bestMove % 9;
			int x = (macro % 3) * 3 + micro % 3;
			int y = (macro / 3) * 3 + micro / 3;

			fprintf( stderr, "%d, %d\n", x,y);
			//CONVERT MOVE BACK
			return std::make_pair(x, y);	
		}
		
		void SetPlayer(char pl){
			this->pl = pl;
		  op = (pl == 1) ? 2 : 1;  
		  fprintf( stderr,"Player is %d\n", pl);
		}

	private:
		float MinMax(char* lmBoard, int move, int depth, int limit, int minMax){
			mNodes++;

		 //  fprintf( stderr,"Macro: %d\n", move % 9);
			// Print();

			char v = this->ValidateBig(lmBoard);

			if(v != 0 && v != -1){
				return -INFINITY * minMax;
			}else if(depth == limit || v == -1){
				return 0.0;
			}

			int macroIndex = move % 9;
			int minLim = 0, maxLim = 81;

			if(lmBoard[macroIndex] == 0){ //Strict macro placement
				minLim = macroIndex * 9;
				maxLim = minLim + 9;
			}

			float extreme = -INFINITY * minMax;

			char* rmBoard = mBoardPool + (depth + 1) * 9;
			
			for(int i = minLim; i < maxLim; i++){
				if(lmBoard[i / 9] != 0){
					i += 9;
					continue; //Skipping macros that is done
				} 

				if(board[i] == 0){
					board[i] = (minMax == 1) ? pl : op;
					// fprintf(stderr, "p, %d\n", i);
					
					for(int j = 0; j < 9; j++){
						rmBoard[j] = lmBoard[j];
					}		
					rmBoard[i / 9] = ValidateSmall(board + (i - i % 9));

					float e = MinMax(rmBoard, i, depth + 1, limit, minMax * -1);
					if(minMax == -1){
						if(e < extreme){
							extreme = e;
						}
					}else{
						if(e > extreme){
							extreme = e;
						}
					}
					
					board[i] = 0;
				}else{
					// fprintf(stderr, "s, %d\n", i);
				}
			}

			return extreme;
		}

		float H(char *lmBoard){
			return(this->ValidateBig(lmBoard));
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

			return -1;
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
						fprintf( stderr, "%d", board[(k / 3) * 18 + k * 3 + i * 9 + j]);
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