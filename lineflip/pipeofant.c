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
 
#define SPI_DEVICE   "/dev/spidev0.0"
#define WIDTH         32
#define HEIGHT        9
#define BLINK_BIT_POS 10
#define MAX_BUF_SIZE  WIDTH * HEIGHT * 4
#define OUTBUF_SIZE   40
#define PIN_DISPLAY_ENABLE RPI_GPIO_P1_22
 
int main(int argc, char *argv[])
{
    int ret = 0;
    int fd;
    int bytes_read;
    uint8_t buf[MAX_BUF_SIZE];
    uint8_t outbuf[OUTBUF_SIZE];
    int x, y, i;
    int inpos, outpos;
 
    int bpp = 1;
 
    uint8_t mode = 0;
    uint8_t bits = 8;
    uint32_t speed = 100000;

    bcm2835_init();
    bcm2835_gpio_fsel(PIN_DISPLAY_ENABLE, BCM2835_GPIO_FSEL_OUTP);
 
/** Usage: give number of bytes per color as command line argument
 *  Example: 'pipeofant 3' for RGB input
 */
 
    if (argc > 1) {
        int bpp_tmp = atoi(argv[1]);
        if (bpp_tmp >= 1 && bpp_tmp <= 4) {
            bpp = bpp_tmp;
        }
    }
 
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
 
    for(;;) {
        bytes_read = read(STDIN_FILENO, buf, WIDTH * HEIGHT * bpp);
 
        if (bytes_read <= 0)
            break;
 
        if (bytes_read != WIDTH * HEIGHT * bpp)
            continue;
 
        memset(outbuf, 0, OUTBUF_SIZE);
        outpos = 0;
 
        for (x=WIDTH-1; x>=0; x--) {
            for (y=HEIGHT-1; y>=0; y--) {
                int on = 0;
                inpos = (y * WIDTH + x) * bpp;
                for (i=0; i<bpp; i++) {
                    if (buf[inpos+i] > 0x30) {
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
 
    return ret;
}
