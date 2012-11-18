#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SPI_DEVICE "/dev/spidev0.0"
#define WIDTH      24
#define HEIGHT     24
#define BUF_SIZE   WIDTH * HEIGHT * 3

int main(int argc, char * argv) {
    int spi_fd;
    int bytes_read;
    uint8_t buf[BUF_SIZE];
    uint8_t outbuf[BUF_SIZE];
    int x, y;

    spi_fd = open(SPI_DEVICE, O_WRONLY);

    if (spi_fd < 0) {
        fprintf(stderr, "Could not open SPI device %s\n", SPI_DEVICE);
        return 1;
    }

    for(;;) {
        bytes_read = read(STDIN_FILENO, buf, BUF_SIZE);

        if (bytes_read <= 0)
            break;

        if (bytes_read != BUF_SIZE)
            continue;

        for (y=0; y<HEIGHT; y++) {
            for (x=0; x<WIDTH; x++) {
                int inpos = (y * WIDTH + x) * 3;
                int outpos;
                if (y % 2 != 0) {
                    outpos = (y * WIDTH + WIDTH - x - 1) * 3;
                } else {
                    outpos = (y * WIDTH + x) * 3;
                }
                outbuf[outpos+0] = buf[inpos+2];
                outbuf[outpos+1] = buf[inpos+1];
                outbuf[outpos+2] = buf[inpos+0];
            }
        }

        write(spi_fd, outbuf, BUF_SIZE);
    }

    close(spi_fd);

    return 0;
}
