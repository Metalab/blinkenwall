#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

struct bl_timer;

struct bl_timer * bl_timer_create();
void bl_timer_free(struct bl_timer * t);

void bl_timer_start(struct bl_timer * t);

/** Elapsed since last timer_start, in microseconds */
uint32_t bl_timer_elapsed(struct bl_timer * t);

#endif
