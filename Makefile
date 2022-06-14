CC = g++

CFLAGS = -g -Wall

all: emulator

emulator: emulator.o chip-8.o
	$(CC) $(CFLAGS) -o emulator emulator.o chip-8.o

emulator.o: emulator.cpp
	$(CC) $(CFLAGS) -c emulator.cpp

chip-8.o: chip-8.cpp chip-8.h
	$(CC) $(CFLAGS) -c chip-8.cpp

clean:
	$(RM) emulator *.o *~
