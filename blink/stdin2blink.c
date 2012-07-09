/* Metalab Blinkenwall interface
 * Reads bytes from STDIN, writes to Raspberry Pi GPIO.
 * Uses SPI protocol for ShiftBrites on Blinkenwall.
 * 
 * Requires BCM 2835 library from:
 * http://www.open.com.au/mikem/bcm2835/
 *
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <wsys@wsys.at>
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
    uint8_t inbuf[BW_WALL_SIZE * 3];

    if (bw_init()) {
        printf("Could not init SPI device (hint: run as root)\n");
        return 1;
    }

    frame = 0;

    while(!feof(stdin)) {
        if (frame++ % (1000 / BW_FRAME_DELAY) == 0)
            bw_wall_config();

        inbytes = fread(inbuf, 1, BW_WALL_SIZE * 3, stdin);

        if (inbytes == BW_WALL_SIZE * 3)
            bw_to_wall(inbuf);

        usleep(BW_FRAME_DELAY * 1000);
    }

    bw_close();

    return 0;
}
