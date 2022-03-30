CC=mpic++
CFLAGS=-std=c++20 -O2
SRC=matrix.hpp distributed_matrix.hpp

target: mpiexample.cpp $(SRC)
	$(CC) -o mpiexample.exe mpiexample.cpp $(CFLAGS)

clean:
	rm -f *.o *.exe