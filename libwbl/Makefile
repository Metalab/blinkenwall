CC      = gcc
CFLAGS  = -Wall -Werror -O3 -I.
DEPS    = libwbl.h
OBJ     = md5.o websocket.o libwbl.o test.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o test
