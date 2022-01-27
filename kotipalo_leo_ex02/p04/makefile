CC=gcc
CFLAGS=-std=c11 -Og -pg -lm -static

main: simpleprofiling.c
	$(CC) -o simpleprofiling.exe simpleprofiling.c $(CFLAGS) 

clean:
	rm -f *.o *.exe *.out