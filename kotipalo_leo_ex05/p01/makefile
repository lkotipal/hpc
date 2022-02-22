CC=g++
CFLAGS=-std=c++20 -Wl,--stack,0x100000000
SRC=

main: main.cpp $(SRC)
	$(CC) -o main.exe main.cpp $(CFLAGS)

clean:
	rm -f *.o *.exe temp*