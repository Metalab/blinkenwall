LIBWBLDIR = ../libwbl
CC      = gcc
CFLAGS  = -Wall -O3 -I../joystick -I.
DEPS    = libwbl.h joystick.h
OBJ     = $(LIBWBLDIR)/md5.o $(LIBWBLDIR)/websocket.o $(LIBWBLDIR)/libwbl.o ../joystick/joystick.o snake.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

snake: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o snake
