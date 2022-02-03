# $Id: Makefile 12 2008-09-15 20:05:15Z aakurone $ 


CC= gcc -O2

HEADERS=global.h
OBJECTS= main.o inout.o physical.o neighbourlist.o solve.o forces.o
TARGET= mdmorse


mdmorse:		$(OBJECTS) 
			$(CC) -o $(TARGET) $(OBJECTS)  -lm


main.o:			main.c $(HEADERS)
			$(CC) -c main.c

inout.o:		inout.c $(HEADERS)
			$(CC) -c inout.c

physical.o:		physical.c $(HEADERS)
			$(CC) -c physical.c

neighbourlist.o:	neighbourlist.c $(HEADERS)
			$(CC) -c neighbourlist.c

solve.o:		solve.c $(HEADERS)
			$(CC) -c solve.c

forces.o:		forces.c $(HEADERS)
			$(CC) -c forces.c

clean:
			rm -f *.o $(TARGET)

