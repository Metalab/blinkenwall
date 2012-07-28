/* Metalab Blinkenwall websocket listener, testing program
 * 
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <wsys@gmx.at>
 */

#include <stdio.h>
#include "libwbl.h"

int main(int argc, char * argv[])
{
    BwlSocketContext * sc;
    int cmd = BW_CMD_NONE;
    char * uuid;

    sc = bw_socket_open();

    bw_wait_for_connections(sc);

    while((cmd = bw_get_cmd_block(sc, &uuid)) !=
          BW_CMD_DISCONNECT) {
        printf("Command: 0x%x, UUID: %s\n",
               (unsigned int)cmd, uuid);
    }

    bw_socket_close(sc);

    return 0;
}
