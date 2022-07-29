CC = g++

CFLAGS = -std=c++0x -g -Wall

all: emulator

emulator: emulator.o chip-8.o peripherals.o
	$(CC) $(CFLAGS) -o emulator emulator.o chip-8.o peripherals.o -I include -L lib -l SDL2-2.0.0

emulator.o: emulator.cpp
	$(CC) $(CFLAGS) -c emulator.cpp

chip-8.o: chip-8.cpp chip-8.h
	$(CC) $(CFLAGS) -c chip-8.cpp

peripherals.o: peripherals.cpp peripherals.h
	$(CC) $(CFLAGS) -c peripherals.cpp

clean:
	$(RM) emulator *.o *~
