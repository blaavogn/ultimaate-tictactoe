bot :
	g++-4.9 -std=c++11 -fmax-errors=3 -Wall -O2 -o bin/$(vers) starter.cpp

human :
	g++-4.9 -std=c++11 -Wall -O2 -o bin/hum Human.cpp

heur :
	g++-4.9 -std=c++11 -Wall -O2 -o bin/heur HeurTest.cpp

debug : 
	g++-4.9 -Wall -std=c++1y -g Human.cpp -O0 -v -da -Q -o bin/hum

moveTest:
	g++ -Wall -std=c++1y -O2 MoveTest.cpp -o  bin/moveTest
tteval : 
	g++-4.9 -Wall -std=c++1y -g TicTacEvalTest.cpp -o bin/tte

clean :
	rm player bot stable_board.o
