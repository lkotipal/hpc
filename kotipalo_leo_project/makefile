CC=mpic++
CFLAGS=-std=c++20 -O2
SRC=point.hpp salesman.hpp route.hpp
OBJ=salesman.o

all: main test

main: main.cpp $(SRC) $(OBJ)
	$(CC) $(CFLAGS) -o salesman.exe main.cpp $(OBJ)

test: main.cpp $(SRC) $(OBJ)
	$(CC) $(CFLAGS) -o test.exe test.cpp $(OBJ)

%.o: %.cpp $(SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.exe