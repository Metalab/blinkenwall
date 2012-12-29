CFLAGS += -I/usr/local/include/ -L/usr/local/lib/

pipeofant: CFLAGS = -lbcm2835 -lrt

VPATH = /usr/local/lib
blinkentrash: CFLAGS = -I/usr/local/include -L/usr/local/lib
blinkentrash: blinkentrash.c -lwiringPi

clean:
	rm -f lineflip pipeofant blinkentrash
