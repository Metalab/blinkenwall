CC      = gcc
CFLAGS  = -Wall -O3 -I.
DEPS    = websocket.h md5.h
OBJ     = websock2stdout.o websocket.o md5.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

websocket2stdout: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o websocket2stdout
