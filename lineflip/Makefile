all: lineflip pipeofant

CFLAGS += -I/usr/local/include/ -L/usr/local/lib/
LIBS += -lbcm2835 -lrt

%: %.c
	gcc ${CFLAGS} -o $@ $< ${LIBS}

clean:
	rm -f lineflip pipeofant
