CC      = gcc
CFLAGS  = -Wall -Werror -O3 -I.
DEPS    = blink.h
OBJ     = blink.o stdin2blink.o
ALIBS   = libbcm2835.a

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

stdin2blink: $(OBJ)
	gcc -o $@ $^ $(ALIBS) $(CFLAGS)

clean:
	rm -f *.o stdin2blink
