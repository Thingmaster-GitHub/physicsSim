CFLAGS = -std=c++17 -O2
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system
SOURCES = run.cpp genaric.cpp main.cpp rendering/*.cpp collisions/*.cpp inputs/*.cpp headers/*.hpp general/*.cpp
testproject: main.cpp
	g++ $(CFLAGS) -o testproject $(SOURCES)  $(LDFLAGS)

.PHONY: test clean

test: testproject
	./testproject

clean:
	rm -f testproject
