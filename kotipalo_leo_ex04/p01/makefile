CC=g++
CFLAGS=-std=c++20
OPT?=0

target: a b

a: a.cpp
	$(CC) -o a.exe a.cpp $(CFLAGS) -O$(OPT)

b: b.cpp
	$(CC) -o b.exe b.cpp $(CFLAGS) -O$(OPT)

clean:
	rm -f *.o *.exe