CC      = gcc
CFLAGS  = -Wall -O3 -I.
DEPS    = ../libwbl/libwbl.h ../blink/blink.h
OBJ     = ../libwbl/md5.o ../libwbl/websocket.o ../libwbl/libwbl.o  \
	      blserver.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

blserver: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o blserver
