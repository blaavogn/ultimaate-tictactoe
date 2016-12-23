class TicTacBoard{
	public: 
		char* board;

	HashBoard(){
		char* board;
	}

	~HashBoard(){
		delete(board);
	}  
};

struct HashBoardEqual {
	bool operator()( HashBoard* const &l,HashBoard* const &r) const {
	  for(int i = 0; i < 9; i++){
	  	if(l->board[i] != r->board[i])
	  		return false;
	  }
	  return true;
	}
};


struct HashBoardHash
{
    std::size_t operator()(HashBoard* const& s) const 
    {	
    	return s->hash;
    }
};