/* Metalab Blinkenwall websocket listener, testing program
 * 
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <wsys@wsys.at>
 */

#include <stdio.h>
#include "libwbl.h"

int main(int argc, char * argv[])
{
    BwlSocketContext * sc;
    int cmd = BW_CMD_NONE;

    sc = bw_socket_open();

    bw_wait_for_connections(sc, BW_DEFAULTTIMEOUT);

    while(cmd != BW_CMD_BUTTON2_RELEASED) {
        int cmd =
            bw_get_cmd_block(sc);
        printf("Command: 0x%x\n", (unsigned int)cmd);
    }

    bw_socket_close(sc);

    return 0;
}
