#pragma once

#include <stdint.h>
#include <unordered_map>

static const std::size_t hash_tt_no[] = {
 0,0,0,0,0,0,0,0,0
};

static const std::size_t hash_tt_pl[] = {
  1,3,9,27,81,243,729,2187,6561
};

static const std::size_t hash_tt_op[] = {
  2,6,18,54,162,486,1458,4374,13122
};

static const std::size_t *hash_tts[]{
	hash_tt_no,hash_tt_pl,hash_tt_op,hash_tt_no
};

class TicTacEval{
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

 	uint64_t *map;

	int *plTmp;
	int *opTmp;
	int *plWin;
	int *opWin;

	public:
		static const uint64_t BM_FULL = 0xffffffffffffffff;
		static const uint64_t BM_EVAL = (BM_FULL & ((1 << 2) - 1));
		static const uint64_t BM_EVAL_LARGE = (BM_FULL & ((1 << 17) - 1));

		static const int plb = 2;
		static const int opb = 22;
		static const int plw = 42;
		static const int opw = 51;
		static const int plcw = 60;
		static const int opcw = 61;
		static const int shft = 2;


		TicTacEval(){
			map = new uint64_t[19683];
			char* board = new char[9];
			plTmp = new int[9];
			opTmp = new int[9];
			plWin = new int[9];
			opWin = new int[9];
			fill(board, 0, 1);
			delete(plTmp);
			delete(opTmp);
			delete(plWin);
			delete(opWin);
		}

		inline uint64_t eval(char *board){
			std::size_t hash = 0;
			hash += hash_tts[(int)board[0]][0];
			hash += hash_tts[(int)board[1]][1];
			hash += hash_tts[(int)board[2]][2];
			hash += hash_tts[(int)board[3]][3];
			hash += hash_tts[(int)board[4]][4];
			hash += hash_tts[(int)board[5]][5];
			hash += hash_tts[(int)board[6]][6];
			hash += hash_tts[(int)board[7]][7];
			hash += hash_tts[(int)board[8]][8];
			return map[hash];
		}

		inline uint64_t evalPl(char *board, int pl){
			const std::size_t *l_hash_tts[4] = {
				hash_tt_no,hash_tt_pl,hash_tt_op, hash_tts[pl]
			};

			std::size_t hash = 0;
			hash += l_hash_tts[(int)board[0]][0];
			hash += l_hash_tts[(int)board[1]][1];
			hash += l_hash_tts[(int)board[2]][2];
			hash += l_hash_tts[(int)board[3]][3];
			hash += l_hash_tts[(int)board[4]][4];
			hash += l_hash_tts[(int)board[5]][5];
			hash += l_hash_tts[(int)board[6]][6];
			hash += l_hash_tts[(int)board[7]][7];
			hash += l_hash_tts[(int)board[8]][8];
			return map[hash];
		}

		inline uint64_t evalPure(std::size_t hash){
			return map[hash];
		}

	private:
		void fill(char* board, int m, int debug = 0){
			if(m==9){
				int v = ValidateSmall(board);
				uint64_t ent = 0;
				uint64_t plBest = 0;
				uint64_t opBest = 0;
				uint64_t plCan = 0;
				uint64_t opCan = 0;
				ent |= v;
				for(int i = 0; i < 9; i++){
					ent |= ((uint64_t) plTmp[i]) << (i * shft + plb);	
					ent |= ((uint64_t) opTmp[i]) << (i * shft + opb);	
					ent |= ((uint64_t) plWin[i]) << (i + plw);	
					ent |= ((uint64_t) opWin[i]) << (i + opw);	
					plBest = std::max((uint64_t)plTmp[i], plBest); 
					opBest = std::max((uint64_t)opTmp[i], opBest); 
					plCan = std::max((uint64_t)plWin[i], plCan);
					opCan = std::max((uint64_t)opWin[i], opCan);
				} 

				ent |= plBest << (9 * shft + plb);
				ent |= opBest << (9 * shft + opb);
				ent |= plCan << plcw;
				ent |= opCan << opcw;
				
				std::size_t hash = 0;
				hash += hash_tts[(int)board[0]][0];
				hash += hash_tts[(int)board[1]][1];
				hash += hash_tts[(int)board[2]][2];
				hash += hash_tts[(int)board[3]][3];
				hash += hash_tts[(int)board[4]][4];
				hash += hash_tts[(int)board[5]][5];
				hash += hash_tts[(int)board[6]][6];
				hash += hash_tts[(int)board[7]][7];
				hash += hash_tts[(int)board[8]][8];

				if(debug){
					printf("%d\n", (int) (ent & BM_EVAL));
					
					printf("Pl: ");
					for(int i = 0; i < 9; i++){
						int plCon = ((ent >> (plb + i * shft)) & BM_EVAL); 
						printf("%d, ",plCon);	
					}
					int plCon = ((ent >> (plb + 9 * shft)) & BM_EVAL); 
					printf("| %d \n",plCon);	
					
					printf("Op: ");
					for(int i = 0; i < 9; i++){
						int opCon = ((ent >> (opb + i * shft)) & BM_EVAL); 
						printf("%d, ",opCon);	
					}
					int opCon = ((ent >> (opb + 9 * shft)) & BM_EVAL); 
					printf("| %d \n",opCon);						

					printf("Pl Win: ");
					for(int i = 0; i < 9; i++){
						int plCon = ((ent >> (i + plw)) & 1); 
						printf("%d, ",plCon);
					}
					printf("\n");

					printf("Op WIN: ");
					for(int i = 0; i < 9; i++){
						int opCon = ((ent >> (i + opw)) & 1); 
						printf("%d, ",opCon);
					}
					printf("\n");

					for(int i = 0; i < 9; i++){
						printf("%d,",board[i]);
					}
					printf("\n\n");
				}

				map[hash] = ent;
			}else{
				for(int i = 0; i < 3; i++){
					board[m] = i;
					fill(board, m+1);
				}
			}
		}

		char ValidateSmall(char *b){
			char l, v;

			for(int i = 0; i < 9; i++){
				plTmp[i] = 0;
				opTmp[i] = 0;
				opWin[i] = 0;
				plWin[i] = 0;
			}

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

			int drawn = 1;
			for(int i = 0; i < 9; i++){
				if(b[i] == 0){
					drawn = 0;
					break;
				}
			}

			if(drawn == 0){
				for(int i = 0; i < 3; i++){
					hDirection(b, plTmp, plWin, X, i * 3, 1); //Horizontal
					hDirection(b, plTmp, plWin, X, i, 3); //Vertical
					hDirection(b, opTmp, opWin, O, i * 3, 1); //Horizontal
					hDirection(b, opTmp, opWin, O, i, 3); //Vertical
				}
				hDirection(b, plTmp, plWin, X, 0, 4);
				hDirection(b, plTmp, plWin, X, 2, 2);
				hDirection(b, opTmp, opWin, O, 0, 4);
				hDirection(b, opTmp, opWin, O, 2, 2);
				return ND;
			}

			return DR;
		}

	public: 
		void hDirection(char *lmBoard, int* tmp, int* win, int o, int base, int step){
			int wpos = 0;
			int c = 4;
			for(int j = 0; j < 3; j++){
				int v = lmBoard[base + j * step];
				
				if((v != 0 && v != o) || v == 4){
					//No possibilities
					return;
				}
				if(v == 0){
					c--;
					wpos = j;
				}
			}
			
			if(c == 3){
				win[base + wpos * step] = 1;
			}

			for(int j = 0; j < 3; j++){
				if(lmBoard[base + j * step] != 0){
					continue;
				}
				tmp[base + j * step] = std::max(tmp[base + j * step], c);		
			}
		}
};
