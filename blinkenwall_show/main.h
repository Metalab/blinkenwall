#ifndef MAIN_H
#define MAIN_H


#define WALL_WIDTH 24
#define WALL_HEIGHT 24
#define WALL_SIZE WALL_WIDTH * WALL_HEIGHT
#define DRAW_MULTIPLICATOR 20
//#define FRAME_DELAY 20

#include <stdint.h>



extern uint8_t Framebuffer[WALL_SIZE*3];

#endif // MAIN_H
