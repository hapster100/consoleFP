all: main.o vec.o matrix.o
	g++ vec.o matrix.o main.o -lncurses -o out

main.o: main.cpp
	g++ -c main.cpp

vec.o: vector2d.cpp
	g++ -c vector2d.cpp -o vec.o

matrix.o: matrix.cpp
	g++ -c matrix.cpp -o matrix.o

cleano:
	rm -rf *.o

clean: cleano
	rm out
