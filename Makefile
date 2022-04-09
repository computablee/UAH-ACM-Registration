CC=clang++
CFLAGS=-std=c++11 -Wall -Wextra -Werror -Wpedantic
LFLAGS=-lncurses -lpthread

all: main.o gui.o
	$(CC) $(LFLAGS) main.o gui.o -o Application

main.o: main.cpp
	$(CC) $(CFLAGS) $< -c -o $@

gui.o: gui.cpp gui.h
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f *.o Application