CC=g++
CFLAGS=-std=c++20 -Wl,--stack,0x10000000
OPT?=0
SRC=matrix.hpp

main: main.cpp $(SRC)
	$(CC) -o main.exe main.cpp $(CFLAGS) -O$(OPT)

clean:
	rm -f *.o *.exe