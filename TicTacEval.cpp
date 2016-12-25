#include <stdint.h>
#include <unordered_map>
#include "TicTacBoard.cpp"

class TicTacEval{
	const int ND = 0;
	const int X = 1;
	const int O = 2;
	const int DR = 3;

	const uint64_t BM_FULL = 0xffffffffffffffff;
	const uint64_t BM_EVAL = BM_FULL & 3;

 	std::unordered_map<int,uint64_t> *map;

	int *plTmp;
	int *opTmp;

	public:
		TicTacEval(){
			map = new std::unordered_map<int, uint64_t>(20000);
			char* board = new char[9];
			plTmp = new int[9];
			opTmp = new int[9];
			fill(board, 0);
			delete(plTmp);
			delete(opTmp);
		}

		long long eval(char* board){
			return map->at(TicTacBoard::Hash(board));
		}

		long long evalMacro(char* board){
			return map->at(TicTacBoard::HashMacro(board));
		}

	private:
		void fill(char* board, int m){
			if(m==9){
				int v = ValidateSmall(board);
				uint ent = 0;
				
				ent |= v;
				for(int i = 0; i < 9; i++){
					ent |= ((uint64_t) plTmp[i]) << (2 * i + 2);	
					ent |= ((uint64_t) opTmp[i]) << (2 * i + 20);	
				} 
				
				// printf("%d\n", v);
				// for(int i = 0; i < 9; i++){
				// 	printf("%d,",board[i]);
				// }
				// printf("\n\n");

				map->insert(std::make_pair(TicTacBoard::Hash(board),ent));
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
					hDirection(b, plTmp, X, i * 3, 1); //Horizontal
					hDirection(b, plTmp, X, i, 3); //Vertical
					hDirection(b, opTmp, O, i * 3, 1); //Horizontal
					hDirection(b, opTmp, O, i, 3); //Vertical
				}
				hDirection(b, plTmp, X, 0, 4);
				hDirection(b, plTmp, X, 2, 2);
				hDirection(b, opTmp, O, 0, 4);
				hDirection(b, opTmp, O, 2, 2);
				return ND;
			}

			return DR;
		}

		void hDirection(char *lmBoard, int* tmp, int o, int base, int step){
			int m = 0;
			int c = 0;

			for(int j = 0; j < 3; j++){
				int v = lmBoard[base + j * step];
				
				if(v == 4){ //hmm
					c=-1;
					break;
				}

				if(v != 0 && v != o){
					//No possibilities
					c=-1;
					break;
				}if(v == 0){
					m += 1 << j;
				}else{
					c++;
				}
			}

			if(c >= 0){
				for(int j = 0; j < 3; j++){
					if((m & (1 << j)) != 0){
						int ind = base + j * step;
						int oldVal = tmp[base + j * step];
						if(oldVal < c + 1){
							tmp[ind] = c + 1;	
						}  
					}
				}
			}
		}
};
