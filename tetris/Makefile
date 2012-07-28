LIBWBLDIR = ../libwbl
CC      = gcc
CFLAGS  = -Wall -O3 -I.
DEPS    = libwbl.h
OBJ     = $(LIBWBLDIR)/md5.o $(LIBWBLDIR)/websocket.o $(LIBWBLDIR)/libwbl.o tetris.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

tetris: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o tetris
