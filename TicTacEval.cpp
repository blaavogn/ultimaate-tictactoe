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
		const uint64_t BM_FULL = 0xffffffffffffffff;
		const uint64_t BM_EVAL = (BM_FULL & ((1 << 2) - 1));

		const int plb = 2;
		const int opb = 20;
		const int plw = 38;
		const int opw = 47;
		const int plcw = 48;
		const int opcw = 49;
		const int shft = 2;


		TicTacEval(){
			map = new uint64_t[19683];
			char* board = new char[9];
			plTmp = new int[9];
			opTmp = new int[9];
			plWin = new int[9];
			opWin = new int[9];
			fill(board, 0);
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

		inline uint64_t evalPure(std::size_t hash){
			return map[hash];
		}

	private:
		void fill(char* board, int m){
			if(m==9){
				int v = ValidateSmall(board);
				uint64_t ent = 0;
				uint64_t plCan = 0;
				uint64_t opCan = 0;
				ent |= v;
				for(int i = 0; i < 9; i++){
					ent |= ((uint64_t) std::min(plTmp[i],3)) << (shft * i + plb);	
					ent |= ((uint64_t) std::min(opTmp[i],3)) << (shft * i + opb);	
					ent |= ((uint64_t) plWin[i]) << (i + plw);	
					ent |= ((uint64_t) opWin[i]) << (i + opw);	
					plCan = std::max((uint64_t)plWin[i], plCan);
					opCan = std::max((uint64_t)opWin[i], opCan);
				} 
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

				// printf("%d\n", ent & BM_EVAL);
				// printf("%d\n", v);
				// printf("Pl: ");
				// for(int i = 0; i < 9; i++){
				// 	int plCon = ((ent >> (i * 2 + 2)) & BM_EVAL); 
				// 	printf("%d, ",plCon);
				// }
				// int plCon = ((ent >> 20) & BM_EVAL); 
				// printf("| %d ",plCon);
				// printf("\n");

				// printf("Op: ");
				// for(int i = 0; i < 9; i++){
				// 	int opCon = ((ent >> (i * 2 + 22)) & BM_EVAL); 
				// 	printf("%d, ",opCon);
				// }
				// int opCon = ((ent >> 40) & BM_EVAL); 
				// printf("| %d ",opCon);
				// printf("\n");

				// for(int i = 0; i < 9; i++){
				// 	printf("%d,",board[i]);
				// }
				// printf("\n\n");

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
			int c = 0;
			for(int j = 0; j < 3; j++){
				int v = lmBoard[base + j * step];
				
				if((v != 0 && v != o) || v == 4){
					//No possibilities
					return;
				}
				if(v == 0){
					c++;
					wpos = j;
				}
			}
			
			if(c == 1){
				win[base + wpos * step] = 1;
			}

			for(int j = 0; j < 3; j++){
				tmp[base + j * step] += 1;		
			}
		}
};
