#pragma once

class TicTacBoard{
	public:
		static std::size_t Hash(char *b){
			std::size_t hash = 0;
			hash |= (b[0] << 0);
			hash |= (b[1] << 2);
			hash |= (b[2] << 4);
			hash |= (b[3] << 6);
			hash |= (b[4] << 8);
			hash |= (b[5] << 10);
			hash |= (b[6] << 12);
			hash |= (b[7] << 14);
			hash |= (b[8] << 16);
			return hash;
		}	
		static std::size_t HashMacro(char *b){
			std::size_t hash = 0;
			hash |= ((b[0]%3) << 0);
			hash |= ((b[1]%3) << 2);
			hash |= ((b[2]%3) << 4);
			hash |= ((b[3]%3) << 6);
			hash |= ((b[4]%3) << 8);
			hash |= ((b[5]%3) << 10);
			hash |= ((b[6]%3) << 12);
			hash |= ((b[7]%3) << 14);
			hash |= ((b[8]%3) << 16);
			return hash;
		}
};
