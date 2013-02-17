#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SPI_DEVICE "/dev/ttyACM0"
#define WIDTH      24
#define HEIGHT     24
#define BUF_SIZE   WIDTH * HEIGHT * 3
#define MIN_DELAY  1000

int
timeval_subtract (result, x, y)
    struct timeval *result, *x, *y;
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}

int main(int argc, char * argv[]) {
    int spi_fd;
    int bytes_read;
    uint8_t buf[BUF_SIZE];
    uint8_t outbuf[BUF_SIZE];
    int i, x, y;
    int send_header = 0;
    struct timeval tv1, tv2, tv_res;

    for(i=1; i<argc; i++) {
        if (strcmp(argv[i], "-header") == 0) {
            send_header = 1;
        }
    }

    spi_fd = open(SPI_DEVICE, O_WRONLY);

    if (spi_fd < 0) {
        fprintf(stderr, "Could not open SPI device %s\n", SPI_DEVICE);
        return 1;
    }

    gettimeofday(&tv1, NULL);

    for(;;) {
        bytes_read = read(STDIN_FILENO, buf, BUF_SIZE);

        if (bytes_read <= 0)
            break;

        if (bytes_read != BUF_SIZE)
            continue;

        gettimeofday(&tv2, NULL);
        timeval_subtract(&tv_res, &tv2, &tv1);

        if (tv_res.tv_usec >= MIN_DELAY) {
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

            if (send_header)
                write(spi_fd, "FNORD", 5);
            write(spi_fd, outbuf, BUF_SIZE);

            gettimeofday(&tv1, NULL);
        }
    }

    close(spi_fd);

    return 0;
}
