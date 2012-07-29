/* Metalab Blinkenwall interface
 * Reads bytes from a named pipe, writes to Raspberry Pi GPIO.
 * Uses SPI protocol for ShiftBrites on Blinkenwall.
 *
 * Data have to be in packed binary 8 bit RGB format,
 * and contain a whole frame (135 bytes for Meatalab Blinkenwall).
 * (e.g. RGBRGBRGBRGB...)
 * 
 * Requires BCM 2835 library from:
 * http://www.open.com.au/mikem/bcm2835/
 *
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <wsys@gmx.at>
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "blink.h"

int main(int argc, char * argv[]) {
    int inbytes;
    int frame;
    int file;
    uint8_t inbuf[BW_WALL_SIZE * 3];

    if (bw_init()) {
        printf("Could not init SPI device (hint: run as root)\n");
        return 1;
    }

    frame = 0;

    if (argc > 1) {
        file = fopen(argc[1], "r");
        if (fd < 0) {
            printf("Error opening blinkenwall pipe");
            return 1;
        }
    } else {
        file = stdin;
    }

    while(!feof(file)) {
        if (frame++ % (1000 / BW_FRAME_DELAY) == 0)
            bw_wall_config();

        inbytes = fread(inbuf, 1, BW_WALL_SIZE * 3, file);

        if (inbytes == BW_WALL_SIZE * 3)
            bw_to_wall(inbuf);

        usleep(BW_FRAME_DELAY * 1000);
    }

    fclose(file);

    bw_close();

    return 0;
}
