CC=gfortran
CFLAGS=-O2 -std=c90 -llapack -Llapackf90/src

target: eigendouble.c
	$(CC) -o eigendouble.exe eigendouble.c $(CFLAGS)

clean:
	rm -f *.o *.exe
