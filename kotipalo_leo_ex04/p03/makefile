CC=g++
CFLAGS=-std=c++20
OPT?=0
SRC=

main: main.cpp $(SRC)
	$(CC) -o main.exe main.cpp $(CFLAGS) -O$(OPT)

clean:
	rm -f *.o *.exe