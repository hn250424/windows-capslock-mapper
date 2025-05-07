CC = gcc
CFLAGS = -Wall -O2
TARGETS = km.exe km_mapper.exe

all: $(TARGETS)

mutex.o: mutex.c mutex.h
	$(CC) $(CFLAGS) -c mutex.c

km.exe: km.c mutex.o
	$(CC) $(CFLAGS) -o km.exe km.c mutex.o

km_mapper.exe: km_mapper.c mutex.o
	$(CC) $(CFLAGS) -o km_mapper.exe km_mapper.c mutex.o

clean:
	del /Q *.exe *.o