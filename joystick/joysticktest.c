#include <stdio.h>
#include "joystick.h"

int main(int argc, char * argv[])
{
    struct controller_handle * h;

    h = open_controller(CONTROLLER_TYPE_JOYSTICK);

    if (!h) {
        printf("Error opening joysticks\n");
        return 1;
    }

    while(1) {
        read_command(h, -1);
    }

    close_controller(h);

    return 0;
}
