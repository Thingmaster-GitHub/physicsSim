CFLAGS = -std=c++17 -O2
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system
FILES = -o testproject main.cpp genaric.cpp run.cpp
DIRECORIES = collisions/*.cpp general/*.cpp inputs/*.cpp rendering/*.cpp headers/*.cpp physics/*.cpp
testproject: main.cpp
	g++ $(CFLAGS) $(FILES) $(DIRECORIES) $(LDFLAGS)

.PHONY: test clean

test: testproject
	./testproject

clean:
	rm -f testproject
