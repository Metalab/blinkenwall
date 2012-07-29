/* Metalab Blinkenwall interface
 * Functions to send RGB data to Blinkenwall.
 * Uses SPI protocol for ShiftBrites.
 * 
 * Requires BCM 2835 library from:
 * http://www.open.com.au/mikem/bcm2835/
 *
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <WSyS@gmx.at>
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bcm2835.h>
#include <blink.h>

#define BSWAP16C(x) (((x) << 8 & 0xff00) | ((x) >> 8 & 0x00ff))
#define BSWAP32C(x) (BSWAP16C(x) << 16 | BSWAP16C((x) >> 16))

/* Data for re-configuring shift brite, sent every second.
 * This also limits electrical current for LEDs
 */
static const uint8_t reset_data[4] = {0b01000000, 0b11110000, 0b00111100, 0b00001111};

/* Alternative configure data with maximum LED current.
 * LEDs behaving weired with this, thus not used
 */
//static const uint8_t reset_data[4] = {0b01000111, 0b11110001, 0b11111100, 0b01111111};

int bw_init() {
    if (!bcm2835_init())
        return 1;

    bcm2835_gpio_fsel(BW_PIN_LATCH, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(BW_PIN_ENABLE, BCM2835_GPIO_FSEL_OUTP);

    bcm2835_spi_begin();

    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);

    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024);

    return 0;
}

void bw_close() {
    bcm2835_spi_end();
}

void bw_latch() {
    bcm2835_gpio_write(BW_PIN_LATCH, HIGH);
    delayMicroseconds(2);
    bcm2835_gpio_write(BW_PIN_LATCH, LOW);
    delayMicroseconds(2);
}

void bw_wall_config() {
    int i;
    uint8_t reset_data_tmp[4];
    for(i=0; i<BW_WALL_SIZE; ++i) {
        memcpy(reset_data_tmp, reset_data, 4);
        bcm2835_spi_transfern((char*)reset_data_tmp, 4);
    }
    bw_latch();
}

void bw_to_wall(const uint8_t * rgb_data) {
    const uint8_t * rgb_data_tmp;
    uint32_t outbuf[BW_WALL_SIZE];
    int x, y;

    rgb_data_tmp = rgb_data;
    for (y=BW_WALL_HEIGHT-1; y>=0; --y) {
        for (x=BW_WALL_WIDTH-1; x>=0; --x) {
            int idx;
            uint32_t val =
                (((uint32_t)rgb_data_tmp[1] << 2) +
                 (rgb_data_tmp[1] >> 6)) |
                (((uint32_t)rgb_data_tmp[0] << 2) +
                 (rgb_data_tmp[0] >> 6)) << 10 |
                (((uint32_t)rgb_data_tmp[2] << 2) +
                 (rgb_data_tmp[2] >> 6)) << 20;

            idx = BW_WALL_HEIGHT * x;
            idx += x%2==0 ? (BW_WALL_HEIGHT-1-y) : y;

            outbuf[idx] = BSWAP32C(val);
            rgb_data_tmp += 3;
        }
    }

    bcm2835_spi_transfern((char*)outbuf, BW_WALL_SIZE * 4);
    bw_latch();
}
