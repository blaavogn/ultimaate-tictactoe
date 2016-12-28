#include <iostream>
#include <unordered_map>
#include <cstring>
#include "HashBoard.cpp"

class TranspositionTable{
 	std::unordered_map <HashBoard*, char, HashBoardHash, HashBoardEqual> *map;
	public:
		TranspositionTable(int initSize = 524287){
			map = new std::unordered_map 
								<HashBoard*, char, HashBoardHash, HashBoardEqual>
								(initSize,HashBoardHash(),HashBoardEqual());
		}

		~TranspositionTable(){
			delete(map);
		}

		void insert(int chosenMove, char* board, std::size_t hash, float val, int prevMove, int itDepth, int movesMade, char cut){
				HashBoard* b = new HashBoard();
				b->board = new char[81];
				for(int i = 0; i < 81; i++){
					b->board[i] = board[i];
				}
				b->hash = hash;
				b->eval = val;
				b->prevMove = prevMove % 9;
				b->itDepth = itDepth;
				b->cut = cut;
				b->movesMade = movesMade;
				map->insert(std::make_pair(b, chosenMove));
		}

		static void UpdateTransPos(std::unordered_map<HashBoard*,char, HashBoardHash, HashBoardEqual>::iterator pos, int bestMove, float val, int itDepth, char cut){
			pos->second = bestMove;
			pos->first->itDepth = itDepth;
			pos->first->eval = val;
			pos->first->cut = cut;
		}

		std::unordered_map<HashBoard*,char, HashBoardHash, HashBoardEqual>::iterator find(HashBoard *key){
			return map->find(key);
		}

		std::unordered_map<HashBoard*,char, HashBoardHash, HashBoardEqual>::iterator end(){
			return map->end();
		}

		std::size_t size(){
			return map->size();	
		}

		void cleanUp(int minMovesMade){
			for (auto it = map->begin(); it != map->end();){
				if(it->first->movesMade < minMovesMade){
					auto oIt = it;
					it = map->erase(it);
					delete(oIt->first);
				}else{
					++it;
				}
			}
		}
};