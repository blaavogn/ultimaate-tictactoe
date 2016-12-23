#include <iostream>
#include "MyBoard.cpp"
#include <unordered_map>

int main(){
	std::unordered_map <MyBoard*, char, MyHash, MyEqual> map(100,MyHash(),MyEqual());;

	char* board1 = new char[81];
	char* board2 = new char[81];
	char* board3 = new char[81];
	char* board4 = new char[81];
	
	board1[1] = -1;
	board2[1] = -1;
	board3[2] = -1;
	board4[2] = 1;

	MyBoard *b1 = new MyBoard();
	b1->board = board1;
	map.insert(std::make_pair(b1, '2'));
	
	auto got = map.find(b1);

	if(got != map.end()){
		fprintf(stderr, "%c\n", got->second);
		got->second = '4';
	}
	got = map.find(b1);

	if(got != map.end()){
		fprintf(stderr, "%c\n", got->second);
	}
}