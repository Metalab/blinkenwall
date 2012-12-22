#include <stdlib.h>
#include <sys/time.h>
#include "timer.h"

struct bl_timer {
    struct timeval tv;
};

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

struct bl_timer * bl_timer_create() {
    struct bl_timer * t = malloc(sizeof(struct bl_timer));
    bl_timer_start(t);
    return t;
}

void bl_timer_free(struct bl_timer * t) {
    free(t);
}

void bl_timer_start(struct bl_timer * t) {
    gettimeofday(&t->tv, NULL);
}

uint32_t bl_timer_elapsed(struct bl_timer * t) {
    struct timeval tv2;
    struct timeval tv_res;

    gettimeofday(&tv2, NULL);
    timeval_subtract(&tv_res, &tv2, &t->tv);

    return tv_res.tv_sec / 1000000 +
        tv_res.tv_usec % 1000000;
}
