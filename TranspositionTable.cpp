#include <iostream>
#include <utility>
#include <unordered_map>
#include <cstring>
#include "HashBoard.cpp"

typedef std::unordered_map <HashKey*, HashBoard*, HashKeyHash, HashKeyEqual> TransMap;

class TranspositionTable{
 	TransMap *map;
	public:
		TranspositionTable(int initSize = 524287){
			map = new TransMap(initSize, HashKeyHash(), HashKeyEqual());
		}

		~TranspositionTable(){
			// delete(map);
		}

		void insert(int chosenMove, uint64_t hash, float val, int valDepth, int movesMade, char shallow, char *board, int prevMove){
			HashBoard *b = new HashBoard();
			b->eval = val;
			b->valDepth = valDepth;
			b->shallow = shallow;
			b->movesMade = movesMade;
			b->bestMove = chosenMove;
			HashKey *k = new HashKey();
			k->hash = hash;
			k->prevMove = prevMove;
			map->insert(std::make_pair(k, b));
		}

		static void UpdateTransPos(TransMap::iterator pos, int bestMove, float val, int valDepth, char shallow){
			pos->second->bestMove = bestMove;
			pos->second->valDepth = valDepth;
			pos->second->eval = val;
			pos->second->shallow = shallow;
		}

		TransMap::iterator find(HashKey* key){
			return map->find(key);
		}

		TransMap::iterator end(){
			return map->end();
		}

		std::size_t size(){
			return map->size();	
		}

		void cleanUp(int minMovesMade){
			for (auto it = map->begin(); it != map->end();){
				if(it->second->movesMade < minMovesMade){
					auto oIt = it;
					it = map->erase(it);
					delete(oIt->second);
				}else{
					++it;
				}
			}
		}
};