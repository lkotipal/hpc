CC=g++
CFLAGS=-std=c++20 -O2 -g -Wl,--stack,0x10000000

target: a b

a: vectorization.cpp
	$(CC) -o a.exe vectorization.cpp $(CFLAGS) -fno-tree-vectorize

b: vectorization.cpp
	$(CC) -o b.exe vectorization.cpp $(CFLAGS) -ftree-vectorize

clean:
	rm -f *.o *.exe