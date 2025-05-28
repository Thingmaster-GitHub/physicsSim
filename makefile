CFLAGS = -std=c++17 -O2
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system

testproject: main.cpp
	g++ $(CFLAGS) -o testproject global.hpp structs.hpp game.hpp run.cpp genaric.cpp main.cpp $(LDFLAGS)

.PHONY: test clean

test: testproject
	./testproject

clean:
	rm -f testproject
