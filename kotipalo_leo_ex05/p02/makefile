CC=gfortran
CFLAGS=-cpp -DRK=8
OPT?=0

target: clean a b

a: omit_frame_pointer.f90
	$(CC) -o a.exe omit_frame_pointer.f90 $(CFLAGS) -O$(OPT) -fno-omit-frame-pointer

b: omit_frame_pointer.f90
	$(CC) -o b.exe omit_frame_pointer.f90 $(CFLAGS) -O$(OPT) -fomit-frame-pointer

clean:
	rm -f *.o *.exe