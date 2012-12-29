#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wiringPiSPI.h>

static void usage(char progname[])
{
	fprintf(stderr, "some_program | %s width height\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Take buffer contents from stdin in chunks of widht * height * 3, and display it on a blinkentrash can.\n");
}

/* copy/pasted from blinkentrash's rgblib.[ch] */
#define SPI_CHANNEL 0
int spi_init(){
	if (wiringPiSPISetup (SPI_CHANNEL, 1000000) < 0){
		printf("SPI Setup error\n");
		return 1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int width, height;
	const int bpp = 3; /* only accepting 3bpp input, as that can be converted in place to an output buffer */
	const int latch = 6;
#define size (width * height * bpp)
#define buffersize (size + latch)
	char *buffer;

	int i;

	if (argc != 3)
	{
		usage(argv[0]);
		exit(1);
	}

	width = atoi(argv[1]);
	height = atoi(argv[2]);

	if (width <= 0 || height <= 0) {
		usage(argv[0]);
		exit(1);
	}

	buffer = malloc(buffersize);
	if (buffer == NULL)
		exit(2);
	memset(buffer, 0xff, latch);

	if (spi_init() != 0)
		exit(3);

	for(;;)
	{
		int bytes_read;
		bytes_read = read(STDIN_FILENO, buffer, size);
		if (bytes_read != size)
			exit(3);

		for (i = 0; i < size; ++i)
		{
			/* from led_set from blinkentrash's rgblib.c */
			buffer[i] = (~buffer[i]) >> 1;
		}

		wiringPiSPIDataRW(SPI_CHANNEL, buffer, buffersize) ;
	}
}
