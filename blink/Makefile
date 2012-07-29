CC      = gcc
CFLAGS  = -Wall -Werror -O3 -I.
DEPS    = blink.h
OBJ     = blink.o pipe2blink.o
ALIBS   = /usr/local/lib/libbcm2835.a

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pipe2blink: $(OBJ)
	gcc -o $@ $^ $(ALIBS) $(CFLAGS)

clean:
	rm -f *.o stdin2blink
