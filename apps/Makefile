CC      = gcc
CFLAGS  = -Wall -g -I../common -I.
DEPS    = common.h config.c config.h ini.c ini.h input.c input.h timer.c timer.h
COMMDIR = ../common
COMMOBJ = $(COMMDIR)/config.o $(COMMDIR)/ini.o $(COMMDIR)/input.o $(COMMDIR)/timer.o

all: snake tetris boom

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

snake: $(COMMOBJ) snake.o
	gcc -o $@ $^ $(CFLAGS)

tetris: $(COMMOBJ) tetris.o
	gcc -o $@ $^ $(CFLAGS)

boom: $(COMMOBJ) boom.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f $(COMMDIR)/*.o
	rm -f *.o snake tetris boom
