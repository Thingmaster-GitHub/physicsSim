CFLAGS = -std=c++17 -O2
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system

testproject: main.cpp
	g++ $(CFLAGS) -o testproject main.cpp game.hpp genaric.cpp run.cpp $(LDFLAGS)

.PHONY: test clean

test: testproject
	./testproject

clean:
	rm -f testproject
