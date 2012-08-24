CC      = gcc
CFLAGS  = -Wall -O3 -I. -std=c99
OBJ     = char2dots.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

char2dots: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o char2dots
