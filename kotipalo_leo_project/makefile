CC=mpic++
CFLAGS=-std=c++20 -O2
SRC=point.hpp salesman.hpp

all: main test

main: main.cpp $(SRC)
	$(CC) $(CFLAGS) -o salesman.exe main.cpp

test: main.cpp $(SRC)
	$(CC) $(CFLAGS) -o test.exe test.cpp

clean:
	rm -f *.o *.exe