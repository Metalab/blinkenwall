#include <stdint.h>

#define CONTROLLER_TYPE_NONE     0
#define CONTROLLER_TYPE_JOYSTICK 1
#define CONTROLLER_TYPE_STDIN    2

#define MAX_JOYSTICKS            16

#define JOYSTICK_DEVICES_DIR     "/dev/input"
#define JOYSTICK_DEVICES_FILTER  "js"

#define IN_TYPE_STDIN            8

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
