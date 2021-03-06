#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <bcm2835.h>
#include <errno.h>
 
#define SPI_DEVICE   "/dev/spidev0.0"
#define WIDTH         (n_panels * 8)
#define HEIGHT        9
#define BLINK_BIT_POS 10
#define MAX_BUF_SIZE  WIDTH * HEIGHT * 4
#define OUTBUF_SIZE   (n_panels * 10)
#define PIN_DISPLAY_ENABLE RPI_GPIO_P1_22
 
int main(int argc, char *argv[])
{
    int ret = 0;
    int fd;
    int bytes_read;
    uint8_t *buf;
    uint8_t *outbuf;
    int x, y, i;
    int inpos, outpos;

    uint8_t threshold = 0x30;
 
    int bpp = 1;
 
    uint8_t mode = 0;
    uint8_t bits = 8;
    uint32_t speed = 200000;

    int n_panels = 4;
    int use_grayscale = 1;

    bcm2835_init();
    bcm2835_gpio_fsel(PIN_DISPLAY_ENABLE, BCM2835_GPIO_FSEL_OUTP);
 
/** Usage: give number of bytes per color as command line argument, and the
 *  number of panels, and whether you want grayscale. (Defaults: 1bpc, 4 panels, grayscale on)
 *  Example: 'pipeofant 3 4 0' for RGB input on four panels without grayscale)
 */
 
    if (argc > 1) {
        bpp = atoi(argv[1]);
        if (bpp < 1 || bpp > 4) exit(1);
    }
    if (argc > 2) {
        n_panels = atoi(argv[2]);
	if (n_panels <= 0) exit(1);
    }
    if (argc > 2 && argv[1] != NULL && argv[1][0] == '0')
        use_grayscale = 0;

    buf = malloc(MAX_BUF_SIZE);
    outbuf = malloc(OUTBUF_SIZE);
    if (buf == NULL || outbuf == NULL)
        exit(2);

    fd = open(SPI_DEVICE, O_WRONLY);
 
    if (fd < 0) {
        fprintf(stderr, "Could not open SPI device %s\n", SPI_DEVICE);
        return 1;
    }
 
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1) {
        printf("MODE %i\n", ret);
        return ret;
    }
  
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1) {
        printf("BITS %i\n", ret);
        return ret;
    }
	
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        printf("SPEED %i\n", ret);
        return ret;
    }

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
 
    for(;;) {
        bytes_read = read(STDIN_FILENO, buf, WIDTH * HEIGHT * bpp);

	if (bytes_read == -1 && errno != EAGAIN)
		exit(3);
 
        if (bytes_read != -1 && bytes_read != WIDTH * HEIGHT * bpp)
            exit(4); /* this should not happen, it's either -1 and EGAIN or nothing at all, otherwise we have a partially updated buffer */

	if (threshold == 0x30)
            usleep(5000);
	if (use_grayscale) {
            if (threshold == 0x20)
                usleep(2000);
            if (threshold == 0x10)
                usleep(1000);

            threshold = (threshold + 0x10) % 0x40;
	}
 
        memset(outbuf, 0, OUTBUF_SIZE);
        outpos = 0;
 
        for (x=0; x<WIDTH; x++) {
            for (y=0; y<HEIGHT; y++) {
                int on = 0;
                inpos = (y * WIDTH + x) * bpp;
                for (i=0; i<bpp; i++) {
                    if (buf[inpos+i] > threshold) {
                        on = 1;
                    }
                }
                if (outpos % BLINK_BIT_POS == 0)
                    outpos++;
                outbuf[outpos / 8] |= on << (7 - (outpos % 8));
                outpos++;

            }
        }
        bcm2835_gpio_write(PIN_DISPLAY_ENABLE, LOW);
        write(fd, outbuf, OUTBUF_SIZE);
        bcm2835_gpio_write(PIN_DISPLAY_ENABLE, HIGH);
    }
 
    close(fd);

    free(buf);
    free(outbuf);
 
    return ret;
}
