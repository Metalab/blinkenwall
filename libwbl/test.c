/* Metalab Blinkenwall websocket listener, testing program
 * 
 * Code by Georg <georg.lippitsch@gmx.at> and Wolfgang <WSyS@gmx.at>
 */

#include <stdio.h>
#include "libwbl.h"

int main(int argc, char * argv[])
{
    BwlSocketContext * sc;
    int cmd = BW_CMD_NONE;
    int connection;
    char * uuid;

    sc = bw_socket_open();

    if (!sc) {
        fprintf(stderr, "Error opening socket\n");
        return 1;
    }

    if (bw_wait_for_connections(sc, NULL) != 0) {
        fprintf(stderr, "Error when waiting for connection\n");
        return 1;
    }

    while((cmd = bw_get_cmd_block(sc, &connection, &uuid)) !=
          BW_CMD_DISCONNECT) {
        printf("Command: 0x%x, Connection: %d, UUID: %s\n",
               (unsigned int)cmd, connection, uuid);
    }

    bw_socket_close(sc);

    return 0;
}
