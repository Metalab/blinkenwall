/* Metalab Blinkenwall interface
 * Header with public functions
 * Uses SPI protocol for ShiftBrites on Blinkenwall.
 * 
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <wsys@wsys.at>
 */

#ifndef BW_BLINK_H
#define BW_BLINK_H

#define BW_WALL_WIDTH  9
#define BW_WALL_HEIGHT 5
#define BW_WALL_SIZE   BW_WALL_WIDTH * BW_WALL_HEIGHT

// Default delay between frames in ms
#define BW_FRAME_DELAY 20

#define BW_PIN_LATCH   RPI_GPIO_P1_22
#define BW_PIN_ENABLE  RPI_GPIO_P1_18

/**
 * Inits GPIO ports and SPI transfer to wall.
 * @return 0 on success, 1 otherwise
 */
int bw_init();

/**
 * De-init transfer to wall
 */
void bw_close();

/**
 * Sends configuration data to ShiftBrites on wall. This should be called
 * before sending data, and in regular intervals to avoid erroneous
 * behavior of the ShiftBrites on the wall.
 */
void bw_wall_config();

/**
 * Sends pixels in rgb_data to wall.
 * @param rgb_data the actual pixel data. The number of bytes in rgb_data
 * has to be 3 * BW_WALL_SIZE, and is in packed 8 bit RGB format.
 * (e.g. RGBRGBRGBRGB...)
 */
void bw_to_wall(const uint8_t * rgb_data);

#endif
