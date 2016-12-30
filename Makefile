bot :
	g++-4.9 -std=c++11 -Wall -O2 -o bin/ch14 starter.cpp

human :
	g++-4.9 -std=c++11 -Wall -O2 -o bin/hum human.cpp

heur :
	g++-4.9 -std=c++11 -Wall -O2 -o bin/heur HeurTest.cpp

debug : 
	g++-4.9 -Wall -std=c++1y -g human.cpp -O0 -v -da -Q -o bin/human

tteval : 
	g++-4.9 -Wall -std=c++1y -g test.cpp -o bin/tte

clean :
	rm player bot stable_board.o
