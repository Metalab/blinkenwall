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
    int swap_rgb = 0;
    int x0 = 0;
    int x1 = WIDTH;
    int y0 = 0;
    int y1 = HEIGHT;
    int hinc = 1;
    int vinc = 1;
    int delay = 0;
    int sleeptime;
    char * outdev = SPI_DEVICE;
    
    struct timeval tv1, tv2, tv_res;

    for(i=1; i<argc; i++) {
        if (strcmp(argv[i], "-header") == 0) {
            send_header = 1;
        }
        else if(strcmp(argv[i], "-rgbswap") == 0) {
            swap_rgb = 1;
        }
        else if(strcmp(argv[i], "-hflip") == 0) {
            x0 = WIDTH-1;
            x1 = -1;
            hinc = -1;
        }
        else if(strcmp(argv[i], "-vflip") == 0) {
            y0 = HEIGHT-1;
            y1 = -1;
            vinc = -1;
        }
        else if(strncmp(argv[i], "-d", 2) == 0) {
            delay = atoi(argv[i]+2) * 1000;
        }
        else if (strncmp(argv[i], "-o", 2) == 0) {
            outdev = argv[i] + 2;
        }
    }

    spi_fd = open(outdev, O_WRONLY);

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

        if (delay) {
            sleeptime = delay - tv_res.tv_usec;
            if (sleeptime > 0) {
                usleep(sleeptime);
            }
        }

        if (tv_res.tv_usec >= MIN_DELAY) {
            int inpos = 0;
            for (y=y0; y!=y1; y+=vinc) {
                for (x=x0; x!=x1; x+=hinc) {
                    int outpos;
                    if (y % 2 != 0) {
                        outpos = (y * WIDTH + WIDTH - x - 1) * 3;
                    } else {
                        outpos = (y * WIDTH + x) * 3;
                    }
                    if (swap_rgb) {
                        outbuf[outpos+0] = buf[inpos+0];
                        outbuf[outpos+1] = buf[inpos+1];
                        outbuf[outpos+2] = buf[inpos+2];
                    } else {
                        outbuf[outpos+0] = buf[inpos+2];
                        outbuf[outpos+1] = buf[inpos+1];
                        outbuf[outpos+2] = buf[inpos+0];
                    }
                    inpos += 3;
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
