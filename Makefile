all:
	g++ --std=c++11 -lncurses -Wdeprecated-writable-strings game.cpp && ./a.out
