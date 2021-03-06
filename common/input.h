#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define CONTROLLER_TYPE_NONE     0
#define CONTROLLER_TYPE_JOYSTICK 1
#define CONTROLLER_TYPE_STDIN    2

#define MAX_JOYSTICKS            16

#define JOYSTICK_DEVICES_DIR     "/dev/input"
#define JOYSTICK_DEVICES_FILTER  "js"

#define IN_TYPE_STDIN            8

#define KEY_UP_K1    'w'
#define KEY_DOWN_K1  's'
#define KEY_LEFT_K1  'a'
#define KEY_RIGHT_K1 'd'
#define KEY_B1_K1    'q'
#define KEY_B2_K1    'e'
#define KEY_UP_K2    'i'
#define KEY_DOWN_K2  'k'
#define KEY_LEFT_K2  'j'
#define KEY_RIGHT_K2 'l'
#define KEY_B1_K2    'u'
#define KEY_B2_K2    'o'
#define KEY_UP        8
#define KEY_DOWN     10
#define KEY_LEFT     11
#define KEY_RIGHT     9
#define KEY_B1       18
#define KEY_B2       19  //TODO: Is this the right value?
#define KEY_QUIT     16
#define KEY_QUIT_K   0x3 // Ctrl+c

struct controller_handle;

struct command
{
    int8_t  controller;       /* which controller the event came from. -1 for no event */
    uint8_t type;             /* event type */
    uint8_t number;           /* axis/button number */
    int16_t value;            /* value of event */
};

/** Opens joystick and keyboard controller
 *  returns: pointer to new controller handle,
 *  or NULL if an error occured.
 */
struct controller_handle * open_controller(int types);

/** Closes opened controllers
 */
void close_controller(struct controller_handle * handle);

/** Blocks until a command arrived or max_dely (in micro seconds)
 *  has elapsed. Returned struct command which contains
 *  information about the event. */
struct command read_command(struct controller_handle * handle,
                            int max_delay);

#endif

